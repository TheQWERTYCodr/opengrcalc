// Copyright 2024 TheQWERTYCoder (theqwertycoder@gmail.com)

#include <array>
#include <exception>
#include <format>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <variant>
#include <list>
#include <vector>
#include <map>
#include <cmath>
#include <stdexcept>
using namespace std;
constexpr bool AUTOFILL_CACHE = false;
typedef int64_t inttype;
typedef uint64_t uinttype;
typedef uint8_t chrtype;
typedef long double fptype;
typedef inttype pricetype;
typedef std::string string;
typedef std::map<inttype,pricetype> pricemap;

class req {
public:
	string name;
	pricemap cache;
	std::map<inttype,const std::exception> invalid;
	pricemap::iterator begin() {return this->cache.begin();}
	pricemap::iterator end() {return this->cache.end();}
	virtual const pricetype get(const inttype x) {return this->cache.at(x);}
	req(const string name) : name(name),cache() {}
	req(const char *name) : name(name),cache() {}
	req() : name(),cache() {}
};
typedef std::shared_ptr<req> reqptr;

class item : public req {
public:
	pricemap prices;
	pricemap bulk;
	bool hasBulk;
	item(const string name, const pricemap prices) : req(name),prices(prices),bulk(),hasBulk(false) {}
	item(const string name, const pricemap prices, const pricemap bulkprices) : req(name),prices(prices),bulk(bulkprices),hasBulk(true) {}
	item(const string name) : req(name),prices(),bulk(),hasBulk(false) {}
	const pricetype get(const inttype x) {
		if (x==0) cache.emplace(x,0);
		try {
			return cache.at(x);
		} catch (const std::exception e) {}
		if (invalid.contains(x)) throw invalid.at(x);
		try {
			auto i = prices.upper_bound(x); // first qty > x
			pricetype y = i->second;
			if (i!=prices.begin()) {
				auto j = i;
				--j;
				const auto [k,v] = *j;
				y = std::ceil(fptype(v*x)/k);
			}
			if (i!=prices.end()) {
				const auto [k,v] = *i;
				y=std::min(y,v);
			}
			if (hasBulk) {
				auto i = bulk.upper_bound(x); // first qty > x
				if (i!=bulk.begin()) {
					auto j = i;
					--j;
					const auto [k,v] = *j;
					if (k>=x) y=std::min(y,v);
					else y=std::min(y,v+get(x-k));
				}
				if (i!=bulk.end()) {
					const auto [k,v] = *i;
					y=std::min(y,v);
				}
			}
			cache.emplace(x,y);
			return y;
		} catch (const std::exception e) {
			invalid.emplace(x,e);
			throw;
		}
	}
};
class multreq : public req {
public:
	reqptr underlying;
	inttype count;
	multreq(string name, reqptr underlying, inttype count) : req(name), underlying(underlying), count(count) {
		if constexpr (AUTOFILL_CACHE) {
			pricemap dup = underlying->cache;
			for (auto x : dup) {
				auto a = x.first/count;
				try {get(a);} catch (std::exception e) {}
				try {get(a+1);} catch (std::exception e) {}
			}
		}
	}
	const pricetype get(const inttype x) {
		try {
			return cache.at(x);
		} catch (std::exception e) {}
		if (invalid.contains(x)) throw invalid.at(x);
		try {
			const pricetype y = underlying->get(x*count);
			cache.emplace(x,y);
			return y;
		} catch (const std::exception e) {
			invalid.emplace(x,e);
			throw;
		}
	}
};
class anyreq : public req {
public:
	std::vector<reqptr> reqs;
	anyreq(string name, std::vector<reqptr> reqs) : req(name), reqs(reqs) {
		if constexpr (AUTOFILL_CACHE) {
			for (auto x : this->reqs) {
				pricemap cache_dup = x->cache;
				for (auto y : cache_dup) {
					get(y.first);
				}
			}
		}
	}
	const pricetype get(const inttype x) {
		try {
			return cache.at(x);
		} catch (std::exception e) {}
		if (invalid.contains(x)) throw invalid.at(x);
		try {
			pricetype y;
			bool flag = true;
			for (auto r : reqs) {
				try {
					y=r->get(x);
					flag = false;
					break;
				} catch (std::exception e) {}
			}
			if (flag) throw std::out_of_range(std::format("anyreq: no available sellers @ qty {}",x));
			for (auto r : reqs) {
				try {
					y=std::min(r->get(x),y);
				} catch (std::exception e) {}
			}
			cache.emplace(x,y);
			return y;
		} catch (const std::exception e) {
			invalid.emplace(x,e);
			throw;
		}
	}
};
class allreq : public req {
public:
	std::vector<reqptr> reqs;
	allreq(string name, std::vector<reqptr> reqs) : req(name), reqs(reqs) {
		if constexpr (AUTOFILL_CACHE) {
			for (auto x : this->reqs) {
				pricemap cache_dup = x->cache;
				for (auto y : cache_dup) {
					try {get(y.first);} catch (std::exception e) {}
				}
			}
		}
	}
	const pricetype get(const inttype x) {
		try {
			return cache.at(x);
		} catch (std::exception e) {}
		if (invalid.contains(x)) throw invalid.at(x);
		try {
			pricetype y = 0;
			for (auto r : reqs) {
				y += r->get(x);
			}
			cache.emplace(x,y);
			return y;
		} catch (std::exception e) {
			invalid.emplace(x,e);
			throw;
		}
	}
};

class tax : public req {
public:
	reqptr underlying;
	fptype mult;
	tax(reqptr x, fptype tax) : req(x->name),underlying(x),mult(1.0l+tax) {
		if constexpr (AUTOFILL_CACHE) {
			for (auto [k,v] : *x) {
				get(k);
			}
		}
	}
	const pricetype get(const inttype x) {
		try {return cache.at(x);} catch (const std::exception e) {}
		if (invalid.contains(x)) throw invalid.at(x);
		try {
			const pricetype y = std::ceil(underlying->get(x)*mult);
			cache.emplace(x,y);
			return y;
		} catch (const std::exception e) {
			invalid.emplace(x,e);
			throw;
		}
	}
};

class constReq : public req {
public:
	pricetype price;
	constReq(std::string name, pricetype x) : req(name), price(x) {}
	const pricetype get(const inttype x) {
		return this->price;
	}
};

auto fromComponents(std::string name, std::vector<std::pair<reqptr,inttype>> a) {
	std::vector<reqptr> x;
	for (auto b : a) {
		if (b.second==0) continue;
		if (b.second==1) {
			x.push_back(reqptr(b.first));
			continue;
		}
		reqptr tmp{new multreq{std::format("{}x {}",b.second,b.first->name),b.first,static_cast<inttype>(b.second)}};
		x.push_back(tmp);
	}
	reqptr tmp = reqptr(new allreq(name,x));
	return tmp;
}

reqptr resistor_1k = reqptr(new item("Resistor - 1K", {
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
reqptr resistor_27 = reqptr(new item("Resistor - 27", {
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
reqptr resistor_10k = reqptr(new item("Resistor - 10K", {
	{1,10},
	{10,20},
	{50,62},
	{100,103},
	{500,350},
	{1000,601}
}, {
	{5000,2180},
	{10000,3860},
	{25000,8275},
	{35000,11270},
	{50000,14950},
	{125000,33875}
}));
reqptr resistor_5k1 = reqptr(new item("Resistor - 5K1", {
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
reqptr resistor_2k = reqptr(new item("Resistor - 2K", {
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
reqptr button = reqptr(new item("Button", {
	{1,16},
	{10,155},
	{25,368},
	{100,1338},
	{250,3145},
	{500,5754}
}, {
	{1500,14453},
	{3000,28104},
	{7500,66240},
	{10500,87119}
}));
reqptr diode = reqptr(new item("Diode", {
	{1,13},
	{10,92},
	{100,494},
	{500,1942},
	{1000,2697},
	{2000,4468}
}, {
	{5000,10560},
	{10000,17980},
	{25000,41575},
	{50000,71900},
	{125000,149500}
}));
reqptr pwr_switch = reqptr(new item("Power Switch", {
	{1,71},
	{10,688},
	{25,1654},
	{100,5742},
	{250,13099},
	{500,24345}
}, {
	{1000,43662},
	{3000,123048},
	{5000,190525},
	{10000,354590}
}));
reqptr usbc = reqptr(new item("USB-C", {
	{1,81},
	{10,688},
	{25,1650},
	{50,3229},
	{100,6177},
	{250,14039}
}, {
	{800,39309},
	{1600,67387},
	{2400,97711},
	{5600,220130},
	{8000,298752},
	{20000,730020}
}));
reqptr edge_port = reqptr(new item("Edge Port", {
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
reqptr thermistor = reqptr(new item("Thermistor", {
	{1,10},
	{5,37},
	{10,63},
	{25,138},
	{50,259},
	{100,441},
	{500,1825},
	{1000,3055},
	{5000,13580}
}, {
	{15000,36285},
	{30000,71280}
}));
reqptr latch = reqptr(new item("D Latch", {
	{1,52},
	{10,418},
	{25,955},
	{100,2844},
	{250,6444},
	{500,10666},
	{1000,15998}
}, {
	{2000,29330},
	{6000,82656},
	{10000,128880},
	{50000,568850}
}));
reqptr display = reqptr(new item("Display", {
	{1,609}
}));
reqptr disp_connector = reqptr(new item("Display Connector", {
	{1,47},
	{10,406},
	{25,927},
	{50,1745},
	{100,3344},
	{250,7633},
	{500,14539},
	{1000,23989}
}, {
	{3000,65424},
	{6000,126492},
	{9000,176652},
	{15000,290055},
	{30000,567030}
}));
reqptr microsd = reqptr(new item("microSD Slot", {
	{1,35},
	{10,348},
	{25,852},
	{50,1602},
	{100,2727},
	{250,6306},
	{500,12272}
}, {
	{1000,23180},
	{5000,112490},
	{10000,218160}
}));
reqptr rp2040 = reqptr(new item("RP2040", {{1,70}}));
reqptr esp32 = reqptr(new item("ESP32-C6-MINI-1-H4", {{1,257}}));
reqptr bat_charger = reqptr(new item("Battery Charger", {
	{1,89},
	{10,780},
	{25,1832},
	{100,5979},
	{250,13885},
	{500,23634},
	{1000,37814}
}, {
	{3000,102807},
	{6000,191436},
	{15000,460860},
	{30000,886290}
}));
reqptr boost_conv = reqptr(new item("Boost Converter", {
	{1,58},
	{10,492},
	{25,1149},
	{100,3676},
	{250,8534},
	{500,14443},
	{1000,22321}
}, {
	{3000,61053},
	{6000,114234},
	{15000,265875},
	{30000,504180}
}));
reqptr flash = reqptr(new item("Flash", {
	{1,74},
	{10,676},
	{25,1669},
	{90,5342},
	{270,15889},
	{540,31309},
	{1080,60378},
	{5040,256637}
}));
reqptr io_exp = reqptr(new item("IO Expander", {
	{1,98},
	{10,873},
	{25,2072},
	{100,6806},
	{250,15906},
	{500,28113}
}, {
	{1500,66584},
	{3000,124290},
	{7500,295193},
	{10500,397730}
}));
reqptr pwr_ctrl = reqptr(new item("Power Controller", {
	{1,38},
	{10,290},
	{25,653},
	{100,1804},
	{250,3798},
	{500,6171},
	{1000,9494}
}, {
	{3000,25635},
	{6000,48420},
	{15000,110370},
	{30000,206490},
	{75000,462825},
	{150000,890100}
}));
reqptr latch_pulse = reqptr(new item("Latch Pulse", {
	{1,87},
	{10,765},
	{25,1797},
	{100,5867},
	{250,13624},
	{500,23190},
	{1000,37103}
}, {
	{3000,100875},
	{6000,187836},
	{15000,452190},
	{30000,869610}
}));
reqptr mode_mux = reqptr(new item("Mode Multiplexer", {
	{1,53},
	{10,451},
	{25,1052},
	{100,3365},
	{250,7812},
	{500,13221},
	{1000,20432}
}, {
	{2000,37260},
	{6000,104568},
	{10000,162260},
	{50000,751200}
}));
reqptr usb_mux = reqptr(new item("USB Multiplexer", {
	{1,94},
	{10,832},
	{25,1953},
	{100,5922},
	{250,12600},
	{500,23940},
	{1000,36539}
}, {
	{4000,136076},
	{8000,262072},
	{12000,377988},
	{28000,873264}
}));
reqptr clock_crystal = reqptr(new item("Clock Crystal 12MHz", {{1,15}}));
reqptr pcbs = reqptr(new item("PCBs", {{1,9778}}, {
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
reqptr battery_1Ah = reqptr(new item("Battery 1Ah", {{1,890}}));
reqptr battery_2Ah = reqptr(new item("Battery 2Ah", {{1,1250}}));
reqptr battery_3Ah = reqptr(new item("Battery 3Ah", {{1,1450}}));
reqptr tariff = reqptr(new item("Digi-Key Tariff", {
	{1,494},
	{5,2221},
	{10,4233},
	{50,19047}
}));
reqptr shipping = reqptr(new constReq("Digi-Key Shipping Cost", 699));
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
auto taxed_components = reqptr(new tax(calculator_components,0.06));
auto calculator = reqptr(new allreq("Calculator",{taxed_components,pcbs}));
int main(int argc, char *argv[]) {
	//pricemap dup = pcbs->prices;
	//for (int k : {1,2,5,10,20,50,100,200,500,1000,2000,5000}) {
		int k = 10;
		auto v = calculator->get(k);
		auto cost_total_fp = fptype(v)/100.0;
		auto cost_per = fptype(v)/fptype(100*k);
		std::cout << std::format("{:4d}x = ${:12.2f} total, ${:12.8f} per\n",k,cost_total_fp,cost_per);
	//}
	return 0;
}