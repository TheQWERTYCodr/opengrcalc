#include <array>
#include <exception>
#include <format>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <string>
#include <variant>
#include <list>
#include <vector>
#include <map>
#include <cmath>
#include <stdexcept>
using namespace std;
typedef long long inttype;
typedef unsigned long long uinttype;
typedef char chrtype;
typedef long double fptype;
typedef inttype pricetype;
typedef std::string string;
class pricemap { // maps quantities to prices
public:
	typedef std::map<inttype,pricetype> underlyingtype;
	std::map<inttype,pricetype> underlying;
	pricetype& get(inttype i) {
		try
		{
			return underlying.at(i);
		}
		catch(const std::exception& e)
		{
			auto iter = underlying.upper_bound(i);
			if (iter==underlying.begin()) {
				throw std::out_of_range(std::format("invalid index {} passed to get",i));
			}
			auto p = *(--iter);
			if (p.first==0) {
				throw std::out_of_range("div by 0");
			}
			this->underlying.insert(std::make_pair(i,p.second*i/pricetype(p.first)));
			return this->underlying[i];
		}
	}
	pricemap() : underlying() {}
	pricemap(std::map<inttype,pricetype> x) : underlying(x) {}
	pricemap(std::initializer_list<std::pair<const inttype,pricetype>> x) : pricemap(std::map<inttype,pricetype>(x)) {}
	pricetype& operator[](inttype i) {
		if (i<0) {
			// maybe we can add sell support in the future, but until then:
			throw std::out_of_range("quantity < 0 in operator[]");
		}
		try
		{
			return get(i);
		}
		catch(const std::out_of_range& e)
		{
			return underlying[i]; // auto-creates ref for us
		}
	}
	std::map<inttype,pricetype>::iterator begin() {return this->underlying.begin();}
	std::map<inttype,pricetype>::iterator end() {return this->underlying.end();}
	int count(inttype idx) {return this->underlying.count(idx);}
};
class req {
public:
	string name;
	pricemap prices;
	virtual std::map<inttype,pricetype>::iterator begin() {return this->prices.begin();}
	virtual std::map<inttype,pricetype>::iterator end() {return this->prices.end();}
	virtual pricetype &get(inttype x) {return this->prices.get(x);}
	pricetype &operator[](int idx) {
		try {
			return this->get(idx);
		} catch (std::exception e) {
			return this->prices[idx];
		}
	}
	req(string name, std::map<inttype,pricetype> prices) : name(name),prices(prices) {}
	req(string name, pricemap prices) : name(name),prices(prices) {}
	req(const char* name, pricemap prices) : name(name),prices(prices) {}
	req(string name) : name(name),prices() {}
	req() : name(),prices() {}
};

class item : public req {
public:
	std::map<inttype,pricetype> bulk;
	std::map<inttype,pricetype> cache;
	bool hasBulk;
	item(string name, pricemap prices) : req(name,prices),hasBulk(false),cache({{0,0}}) {}
	item(string name, pricemap prices, std::map<inttype,pricetype> bulkprices) : req(name,prices),bulk(bulkprices),hasBulk(true),cache({{0,0}}) {}
	item(string name) : req(name),hasBulk(false),cache({{0,0}}) {}
	pricetype &get(inttype x) {
		try {return cache.at(x);} catch (std::exception e) {}
		pricetype y = prices.get(x);
		if (hasBulk) {
			auto i = bulk.upper_bound(x);
			if (i!=bulk.end()) {
				auto [k,v] = *i;
				y=std::min(y,v);
			}
			if (i!=bulk.begin()) {
				--i;
				auto [k,v] = *i;
				y=std::min(y,v+get(x-k));
			}
		}
		cache[x]=y;
		return cache[x];
	}
};
class multreq : public req {
public:
	std::shared_ptr<req> underlying;
	inttype count;
	multreq(string name, std::shared_ptr<req> underlying, inttype count) : req(name), underlying(underlying), count(count) {
		pricemap dup = underlying->prices;
		for (auto x : dup) {
			auto a = x.first/count;
			try {get(a);} catch (std::exception e) {}
			try {get(a+1);} catch (std::exception e) {}
		}
	}
	pricetype &get(inttype x) {
		try {
			return this->prices.underlying.at(x);
		} catch (std::exception e) {
			prices[x] = this->underlying->get(x*count);
			return prices[x];
		}
	}
};
class anyreq : public req {
public:
	std::vector<req*> reqs;
	anyreq(string name, std::vector<req*> reqs) : req(name), reqs(reqs) {
		for (auto x : this->reqs) {
			pricemap prices_dup = x->prices;
			for (auto y : prices_dup) {
				get(y.first);
			}
		}
	}
	pricetype &get(inttype x) {
		try {
			return this->prices.underlying.at(x);
		} catch (std::exception e) {
			pricetype y;
			for (auto r : reqs) {
				try {
					y=r->get(x);
					break;
				} catch (std::exception e) {
					continue;
				}
			}
			for (auto r : reqs) {
				try {
					y=std::min(r->get(x),y);
				} catch (std::exception e) {
					continue;
				}
			}
			prices[x]=y;
			return prices[x];
		}
	}
};
class allreq : public req {
public:
	std::vector<std::shared_ptr<req>> reqs;
	allreq(string name, std::vector<std::shared_ptr<req>> reqs) : req(name), reqs(reqs) {
		for (auto x : this->reqs) {
			pricemap prices_dup = x->prices;
			for (auto y : prices_dup) {
				try {get(y.first);} catch (std::exception e) {}
			}
		}
	}
	pricetype &get(inttype x) {
		try {
			return prices.underlying.at(x);
		} catch (std::exception e) {
			pricetype y = 0;
			for (auto r : reqs) {
				y += r->get(x);
			}
			prices[x]=y;
			return prices[x];
		}
	}
};

class tax : public req {
public:
	std::shared_ptr<req> underlying;
	fptype mult;
	tax(std::shared_ptr<req> x, fptype tax) : req(x->name),underlying(x),mult(1.0l+tax) {
		for (auto [k,v] : *x) {
			get(k);
		}
	}
	pricetype &get(inttype x) {
		pricetype tmp = std::ceil(underlying->get(x)*mult);
		prices[x]=tmp;
		return prices[x];
	}
};

class constReq : public req {
public:
	pricetype price;
	constReq(std::string name, pricetype x) : req(name,pricemap({{1,x}})), price(x) {}
	virtual pricetype &get(inttype x) override {
		return this->price;
	}
};

auto fromComponents(std::string name, std::vector<std::pair<std::shared_ptr<req>,inttype>> a) {
	std::vector<std::shared_ptr<req>> x;
	for (auto b : a) {
		if (b.second==0) continue;
		if (b.second==1) {
			x.push_back(std::shared_ptr<req>(b.first));
			continue;
		}
		std::shared_ptr<req> tmp{new multreq{std::format("{}x {}",b.second,b.first->name),b.first,static_cast<inttype>(b.second)}};
		x.push_back(tmp);
	}
	std::shared_ptr<req> tmp = std::shared_ptr<req>(new allreq(name,x));
	return tmp;
}

std::vector<item> items;
std::shared_ptr<req> resistor_1k = std::shared_ptr<req>(new item("Resistor - 1K", {
	{1,10},
	{10,20},
	{50,63},
	{100,105},
	{500,356},
	{1000,612}
}, {
	{5000,2225},
	{10000,3940},
	{25000,8425},
	{35000,11480},
	{50000,15250},
	{125000,34625}
}));
std::shared_ptr<req> resistor_27 = std::shared_ptr<req>(new item("Resistor - 27", {
	{1,10},
	{10,21},
	{50,66},
	{100,110},
	{500,372},
	{1000,639}
}, {
	{5000,2320},
	{10000,4100},
	{25000,8800},
	{35000,11970},
	{50000,15900},
	{125000,36000}
}));
std::shared_ptr<req> resistor_10k = std::shared_ptr<req>(new item("Resistor - 10K", {
	{1,10},
	{10,20},
	{50,62},
	{100,103},
	{500,350},
	{1000,601}
}));
std::shared_ptr<req> resistor_5k1 = std::shared_ptr<req>(new item("Resistor - 5K1", {
	{1,10},
	{10,21},
	{50,66},
	{100,110},
	{500,372},
	{1000,639}
}));
std::shared_ptr<req> resistor_2k = std::shared_ptr<req>(new item("Resistor - 2K", {
	{1,10},
	{10,21},
	{50,66},
	{100,110},
	{500,372},
	{1000,639}
}));
std::shared_ptr<req> button = std::shared_ptr<req>(new item("Button", {
	{1,15},
	{10,146},
	{25,346},
	{100,1258},
	{250,2956},
	{500,5408}
}));
std::shared_ptr<req> diode = std::shared_ptr<req>(new item("Diode", {
	{1,13},
	{10,92},
	{100,494},
	{500,1942},
	{1000,2697},
	{2000,4468}
}));
std::shared_ptr<req> pwr_switch = std::shared_ptr<req>(new item("Power Switch", {
	{1,71},
	{10,688},
	{25,1654},
	{100,5742},
	{250,13099},
	{500,24345}
}));
std::shared_ptr<req> usbc = std::shared_ptr<req>(new item("USB-C", {
	{1,81},
	{10,688},
	{25,1650},
	{50,3229},
	{100,6177},
	{250,14039}
}));
std::shared_ptr<req> edge_port = std::shared_ptr<req>(new item("Edge Port", {
	{1,406},
	{10,3617},
	{25,8996},
	{50,17808},
	{100,32110},
	{250,79813},
	{500,147631},
	{1000,263891},
	{2500,608980}
}));
std::shared_ptr<req> thermistor = std::shared_ptr<req>(new item("Thermistor", {
	{1,10},
	{5,37},
	{10,63},
	{25,138},
	{50,259},
	{100,441},
	{500,1825},
	{1000,3055},
	{5000,13580}
}));
std::shared_ptr<req> latch = std::shared_ptr<req>(new item("D Latch", {
	{1,52},
	{10,418},
	{25,955},
	{100,2844},
	{250,6444},
	{500,10666},
	{1000,15998}
}));
std::shared_ptr<req> display = std::shared_ptr<req>(new item("Display", {
	{1,609}
}));
std::shared_ptr<req> disp_connector = std::shared_ptr<req>(new item("Display Connector", {
	{1,47},
	{10,406},
	{25,927},
	{50,1745},
	{100,3344},
	{250,7633},
	{500,14539},
	{1000,23989}
}));
std::shared_ptr<req> microsd = std::shared_ptr<req>(new item("microSD Slot", {
	{1,35},
	{10,348},
	{25,852},
	{50,1602},
	{100,2727},
	{250,6306},
	{500,12272}
}));
std::shared_ptr<req> rp2040 = std::shared_ptr<req>(new item("RP2040", {{1,70}}));
std::shared_ptr<req> esp32 = std::shared_ptr<req>(new item("ESP32-C6-MINI-1-H4", {{1,257}}));
std::shared_ptr<req> bat_charger = std::shared_ptr<req>(new item("Battery Charger", {
	{1,89},
	{10,780},
	{25,1832},
	{100,5979},
	{250,13885},
	{500,23634},
	{1000,37814}
}));
std::shared_ptr<req> boost_conv = std::shared_ptr<req>(new item("Boost Converter", {
	{1,58},
	{10,492},
	{25,1149},
	{100,3676},
	{250,8534},
	{500,14443},
	{1000,22321}
}));
std::shared_ptr<req> flash = std::shared_ptr<req>(new item("Flash", {
	{1,74},
	{10,676},
	{25,1669},
	{90,5342},
	{270,15889},
	{540,31309},
	{1080,60378},
	{5040,256637}
}));
std::shared_ptr<req> io_exp = std::shared_ptr<req>(new item("IO Expander", {
	{1,98},
	{10,873},
	{25,2072},
	{100,6806},
	{250,15906},
	{500,28113}
}));
std::shared_ptr<req> pwr_ctrl = std::shared_ptr<req>(new item("Power Controller", {
	{1,38},
	{10,290},
	{25,653},
	{100,1804},
	{250,3798},
	{500,6171},
	{1000,9494}
}));
std::shared_ptr<req> latch_pulse = std::shared_ptr<req>(new item("Latch Pulse", {
	{1,87},
	{10,765},
	{25,1797},
	{100,5867},
	{250,13624},
	{500,23190},
	{1000,37103}
}));
std::shared_ptr<req> mode_mux = std::shared_ptr<req>(new item("Mode Multiplexer", {
	{1,53},
	{10,451},
	{25,1052},
	{100,3365},
	{250,7812},
	{500,13221},
	{1000,20432}
}));
std::shared_ptr<req> usb_mux = std::shared_ptr<req>(new item("USB Multiplexer", {
	{1,94},
	{10,832},
	{25,1953},
	{100,5922},
	{250,12600},
	{500,23940},
	{1000,36539}
}));
std::shared_ptr<req> clock_crystal = std::shared_ptr<req>(new item("Clock Crystal 12MHz", {{1,15}}));
std::shared_ptr<req> pcbs = std::shared_ptr<req>(new item("PCBs", {
	{5,9778},
	{10,10670},
	{15,12146},
	{20,13138},
	{25,14590},
	{30,15376},
	{40,18287},
	{50,20689},
	{75,26654},
	{100,31193},
	{125,35945},
	{150,41435},
	{200,51738},
	{250,60866},
	{300,71489},
	{350,76553},
	{400,79144},
	{450,87885},
	{500,96914},
	{600,110964},
	{700,128061},
	{800,143948},
	{900,161600},
	{1000,179113},
	{1500,239790},
	{2000,317717},
	{2500,390373},
	{3000,473760},
	{3500,552097},
	{4000,629826},
	{4500,708140},
	{5000,785869},
	{5500,863836},
	{6000,942132},
	{6500,1019880},
	{7000,1098175},
	{7500,1175926},
	{8000,1254240},
	{9000,1409915},
	{10000,1565980}
}));
std::shared_ptr<req> battery_1Ah = std::shared_ptr<req>(new item("Battery 1Ah", {{1,890}}));
std::shared_ptr<req> battery_2Ah = std::shared_ptr<req>(new item("Battery 2Ah", {{1,1250}}));
std::shared_ptr<req> battery_3Ah = std::shared_ptr<req>(new item("Battery 3Ah", {{1,1450}}));
std::shared_ptr<req> tariff = std::shared_ptr<req>(new item("Digi-Key Tariff", {
	{5,2221},
	{50,19047}
}));
std::shared_ptr<req> shipping = std::shared_ptr<req>(new constReq("Digi-Key Shipping Cost", 699));
auto calculator_components = fromComponents("Calculator Components", {
	{resistor_1k,2},
	{resistor_27,2},
	{resistor_10k,6},
	{resistor_5k1,2},
	{resistor_2k,4},
	{button,67},
	{diode,65},
	{pwr_switch,1},
	{usbc,1},
	{edge_port,2},
	{thermistor,1},
	{latch,1},
	{display,1},
	{disp_connector,1},
	{microsd,1},
	{rp2040,1},
	{esp32,1},
	{bat_charger,1},
	{boost_conv,1},
	{flash,1},
	{io_exp,2},
	{pwr_ctrl,2},
	{latch_pulse,1},
	{mode_mux,1},
	{usb_mux,1},
	{clock_crystal,1},
	{battery_3Ah,1},
	//{tariff,1},
	//{shipping,1}
});
auto taxed_components = std::shared_ptr<req>(new tax(calculator_components,0.06));
auto calculator = std::shared_ptr<req>(new allreq("Calculator",{taxed_components,pcbs}));
int main(int argc, char *argv[]) {
	pricemap dup = pcbs->prices;
	for (auto [k,_] : dup) {
		auto v = calculator->get(k);
		auto cost_total_fp = fptype(v)/100.0;
		auto cost_per = fptype(v)/fptype(100*k);
		std::cout << std::format("{:8d} calculators = ${:10.2f} total, ${:12.9f} per\n",k,cost_total_fp,cost_per);
	}
	return 0;
}