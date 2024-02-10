#include <array>
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
                throw std::out_of_range("invalid index passed to get");
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
    pricetype& operator[](int idx) {return this->prices[idx];}
    req(string name, std::map<inttype,pricetype> prices) : name(name),prices(prices) {}
    req(string name, pricemap prices) : name(name),prices(prices) {}
    req(const char* name, pricemap prices) : name(name),prices(prices) {}
    req(string name) : name(name),prices() {}
    req() : name(),prices() {}
};
class item : public req {
public:
    item(string name, pricemap prices) : req(name,prices) {}
    item(string name) : req(name) {}
};
class multreq : public req {
public:
    req underlying;
    inttype count;
    multreq(string name, req underlying, inttype count) : req(name), underlying(underlying), count(count) {
        for (auto x : this->underlying.prices.underlying) {
            auto a = x.first/count;
            this->prices[a] = 0;
            this->prices[a+1] = 0;
        }
        for (auto x : this->prices) {
            this->prices[x.first] = this->underlying[x.first*count];
        }
    }
};
class anyreq : public req {
public:
    std::vector<req> reqs;
    anyreq(string name, std::vector<req> reqs) : req(name), reqs(reqs) {
        for (auto x : this->reqs) {
            for (auto y : x.prices) {
                if (this->prices.underlying.count(y.first)) {
                    this->prices[y.first] = min(this->prices[y.first],y.second);
                } else {
                    this->prices[y.first] = y.second;
                }
            }
        }
    }
};
class allreq : public req {
public:
    std::vector<req> reqs;
    allreq(string name, std::vector<req> reqs) : req(name), reqs(reqs) {
        for (auto x : this->reqs) {
            for (auto y : x.prices) {
                prices[y.first] = 0;
            }
        }
        auto prices_dup = this->prices; // copy because otherwise it segfaults
        for (auto x : prices_dup) {
            for (auto y : this->reqs) {
                try
                {
                    prices[x.first] += y.prices.get(x.first);
                }
                catch(const std::exception& e) // the quantity is below the MOQ, so we erase the price for that quantity
                {
                    prices.underlying.erase(x.first);
                    break;
                }
            }
        }
    }
};
std::vector<item> items;
int main(int argc, char *argv[]) {
    auto proc_1 = item("esp32-s3-wroom-1u",pricemap({{1,300},{10,2800},{100,25000}}));
    auto proc_any = anyreq("any processor",{proc_1});
    auto coproc_any = anyreq("any coprocessor",{proc_1});
    auto qspi_1 = item("W25Q256JVFIQ 256Mb QSPI flash",pricemap({{1,10},{100,900},{1000,8000}}));
    auto qspi_2 = item("25Q128JVSIQ 128Mb QSPI flash",pricemap((pricemap::underlyingtype){{1,10}}));
    auto qspi_2_256Mb = multreq("2x 25Q128JVSIQ 128Mb QSPI flash",qspi_2,2);
    auto qspi_any = anyreq("any qspi flash",{qspi_1,qspi_2_256Mb});
    auto shiftregister_1 = item("SN74HC595 shift register",pricemap({{1,105},{25,2500},{100,9500}}));
    auto shiftregister_any = anyreq("any shift register",{shiftregister_1});
    auto lcd_1 = item("2*1.5” TFT LCD ST7789",pricemap({{5,1360},{200,52400},{2000,516000}}));
    auto lcd_any = anyreq("any lcd",{lcd_1});
    auto pcb_1 = item("PCB from PCBWay",pricemap({{5,4786},{10,5435},{25,7615},{100,18451},{200,32796},{500,71803},{1000,115223},{2000,214900},{5000,508231},{10000,1014775},{20000,2027845},{50000,5067453},{100000,10133197}}));
    auto pcb_any = anyreq("any pcb",{pcb_1});
    auto btn_1 = item("6x6x6 sparkfun button COM-00097",pricemap({{1,45},{25,1075},{100,4100}}));
    auto btn_2 = item("6x6x6 SMD button",pricemap((pricemap::underlyingtype){{2000,2000}}));
    auto btn_3 = item("12x12 sparkfun button",pricemap({{1,55},{25,1300},{100,5000}}));
    auto btn_4 = item("ebay buttons",pricemap({{10,509},{50,620},{100,764},{200,1056},{500,1929}}));
    auto btn_any = anyreq("any button",{btn_1,btn_2,btn_3,btn_4});
    auto btn_x128 = multreq("128x any button",btn_any,128);
    auto calculator_1 = allreq("calculator w/ proc, coproc, 256Mb QSPI, shift register, 128 btns, lcd, and pcb",{proc_any,coproc_any,qspi_any,shiftregister_any,btn_x128,lcd_any,pcb_any});
    for (int count : {5,10,20,50,100,200,500,1000,2000,5000,10000,20000,50000,100000,200000,500000}) {
        auto cost_total = calculator_1[count];
        auto cost_total_fp = fptype(cost_total)/100.0;
        auto cost_per = fptype(cost_total)/fptype(100*count);
        printf("cost of %d calculators = $%.2Lf total, $%LF per calc\n",count,cost_total_fp,cost_per);
    }
    return 0;
}