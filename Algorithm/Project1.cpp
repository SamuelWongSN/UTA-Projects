#include <iostream>
#include <numeric>
#include <math.h>
#include <time.h>
#include <string>
#include <vector>

using namespace std;
class BitString
{
    private:
    string bit;
    public:
    	static int recursive_call_cnt;
    	static int recursive_call_cnt2;
    	static int addition_cnt;
    	static int shift_cnt;
        static void unified(string& str){
            int pos = 0;
            for(;pos < str.length()-1; pos++)
            {
                if('0' != str[pos]) break;
            }
            str = str.erase(0,pos);
        };
        static void evenUp(BitString& s1, BitString& s2){
            int len1 = s1.getlen(), len2 = s2.getlen();
            int len = len1 > len2 ? len1 : len2;
            if(len1 < len){
                s1.fillHead(len-len1);
            }
            else if(len2 < len){
                s2.fillHead(len-len2);
            }
        };
        
        BitString(){
        };
        BitString(string& str){
            this->bit = str;
        };
        BitString(const char* conChar){
            string str(conChar);
            this->bit = str;
        };
        void setstr(string str){
            this->bit = str;
        }
        string getstr(){
            BitString::unified(this->bit);
            return this->bit;
        };
        int getlen(){
            return this->bit.length();
        }
        void selfunified(){
            BitString::unified(this->bit);
        }
        void shfit_(int n){
            this->fillTail(n);
        }
        BitString shfit(int n){
            BitString res(this->bit);
            res.fillTail(n);
            return res;
        }

    private:
        void tickTop(){
            this->bit.erase(0,1);
        }
        void fillHead(int n){
            string filled(n,'0');
            this->bit = filled + this->bit;
        };
        void fillTail(int n){
        	BitString::shift_cnt++;
            string filled(n,'0');
            this->bit = this->bit + filled;
        };
        vector<BitString> split(){
            string s = this->bit;
            int len = s.length();
            int mid = len / 2;
            string s1 = s.substr(0,mid);
            string s2 = s.substr(mid,len);
            vector<BitString> ans;
            BitString bit1(s1);
            BitString bit2(s2);
            if("" != s1)
                ans.push_back(s1);
            ans.push_back(s2);
            return ans;
        }

        int singleBit(){
            BitString tmp(this->bit);
            tmp.selfunified();
            if(1 != tmp.getlen()) return -1;
            return (tmp.bit[0] - '0');
        }
        BitString getInverse(){
            int len = this->getlen();
            string res(len,'0');
            for(int i = 0 ; i < len; i++)
            {
                if('0' == this->bit[i]) res[i] = '1';
            }
            return BitString(res);
        }

    public:
        friend BitString operator+ (const BitString& bit1, const BitString& bit2);
        friend BitString operator- (const BitString& bit1, const BitString& bit2);
        friend BitString operator* (const BitString& bit1, const BitString& bit2);
        friend ostream& operator<< (ostream& os, const BitString& bit);
        friend istream& operator>> (istream& is, BitString& bit);


        static BitString log3_Mult(const BitString& bit1, const BitString& bit2){
        	BitString::recursive_call_cnt2++;
            BitString b1 = bit1;
            BitString b2 = bit2;
            BitString::evenUp(b1,b2);
            if(-1 != b1.singleBit()) return b1.singleBit() ? b2 : b1;
            if(-1 != b2.singleBit()) return b2.singleBit() ? b1 : b2;
    
            vector<BitString> A_B = b1.split();
            vector<BitString> C_D = b2.split();
            int n = A_B[1].getlen();
            BitString AC = BitString::log3_Mult(A_B[0], C_D[0]);
            BitString BD = BitString::log3_Mult(A_B[1], C_D[1]);
            BitString AplusB = A_B[0] + A_B[1];
            BitString CplusD = C_D[0] + C_D[1];
            BitString AplusB_CplusD = BitString::log3_Mult(AplusB, CplusD);

            BitString ans = AC.shfit(n*2) +AplusB_CplusD.shfit(n) + BD -AC.shfit(n) - BD.shfit(n);
            return ans;
        }
        static void cnt_init(){
        	BitString::recursive_call_cnt = 0;
        	BitString::addition_cnt = 0;
        	BitString::shift_cnt = 0;
        	BitString::recursive_call_cnt2 = 0;
		}
		static void cnt_out(vector<int>& add_cnt, vector<int>& shfit_cnt, vector<int>& recursive_cnt, int flag){
            if (flag == 2) recursive_cnt.push_back(BitString::recursive_call_cnt);
            else recursive_cnt.push_back(BitString::recursive_call_cnt2);
            add_cnt.push_back(BitString::addition_cnt);
            shfit_cnt.push_back(BitString::shift_cnt);
		}
        static void cnt_out_value(int flag)
        {
            if(flag == 2)
            {
                cout << "========normal integer multiplication========" << endl;
                cout << "Recursive call:"<< BitString::recursive_call_cnt << endl;
            }
            else
            {
                cout << "========better integer multiplication========" << endl;
                cout << "Recursive call:"<< BitString::recursive_call_cnt2 << endl;
            }
            cout << "Addition Operation" << BitString::addition_cnt << endl;
            cout << "Shift Operation" << BitString::shift_cnt << endl;
        }
};


BitString operator+ (const BitString& bit1, const BitString& bit2){
	BitString::addition_cnt++;
    BitString b1 = bit1;
    BitString b2 = bit2;
    BitString::evenUp(b1,b2);
    int len = b1.getlen();
    string ans(len,'0');
    int pos = len-1;
    int carry = 0;
    while(pos >= 0){
        int num1 = b1.bit[pos] - '0';
        int num2 = b2.bit[pos] - '0';
        int sum = num1 + num2 + carry;
        ans[pos] = sum%2 + '0';
        carry = sum / 2;
        pos--;
    }
    if (1 == carry){
        ans = "1" + ans;
    }
    return BitString(ans);
};

BitString operator- (const BitString& bit1, const BitString& bit2){
    BitString b1 = bit1;
    BitString b2 = bit2;
    BitString _1("1");
    BitString::evenUp(b1,b2);
    b2 = b2.getInverse();
    int len_before = b2.getlen();
    b2 = b2 + _1;
    BitString res = b1 + b2;
    int len_after = res.getlen();
    if(len_after > len_before) res.tickTop();
    return res;

    
}
BitString operator* (const BitString& bit1, const BitString& bit2){
	BitString::recursive_call_cnt++;
    BitString b1 = bit1;
    BitString b2 = bit2;
    b1.selfunified();
    b2.selfunified();
    if(-1 != b1.singleBit()) return b1.singleBit() ? b2 : b1;
    if(-1 != b2.singleBit()) return b2.singleBit() ? b1 : b2;
    vector<BitString> A_B = b1.split();
    vector<BitString> C_D = b2.split();
    int shift_AC = A_B[1].getlen() + C_D[1].getlen();
    int shift_AD = A_B[1].getlen();
    int shift_BC = C_D[1].getlen();
    BitString AC = A_B[0] * C_D[0];
    BitString AD = A_B[0] * C_D[1];
    BitString BC = A_B[1] * C_D[0];
    BitString BD = A_B[1] * C_D[1];
    AC.shfit_(shift_AC);
    BC.shfit_(shift_BC);
    AD.shfit_(shift_AD);
    BitString ans = AC + AD + BC + BD;
    return ans;
}

ostream& operator<< (ostream& os, const BitString& bit)
    {
        os << bit.bit;
        return os;
    }
istream& operator>> (istream& is, BitString& bit)
    {
        string bitstr;
        is >> bitstr;
        bit.setstr(bitstr);
        return is;
    }

vector<vector<string>> make_test_samples(int n)
{
    vector<vector<string>>test_samples;
    for(int i=0;i<=n-1;i++)
    {
        vector<string>samples;
        for(int k=10;k>0;k--)
        {
            string sample("");
            for(int j=1;j<=pow(2, i+1);j++)
            {
                sample += (rand()%10 > 4) ? '1' : '0';
            }
            samples.push_back(sample);
        }
        test_samples.push_back(samples);
    }
    return test_samples;
}

int cal_avg(vector<int>& list)
{
    int mean = 0;
    int sum = accumulate(begin(list), end(list), 0);
    int n = list.size();
    mean = sum / list.size();
    return mean;
}

void test(BitString bit1, BitString bit2)
{
    BitString::cnt_init();
    clock_t t1 = clock();
    BitString ans = bit1 * bit2;
    ans.selfunified();
    int n2t = double(clock() - t1) * 1000 / CLOCKS_PER_SEC;
    BitString::cnt_out_value(2);
    cout << "Executed Time:" << n2t << endl;
    
    BitString::cnt_init();
    t1 = clock();
    ans = BitString::log3_Mult(bit1, bit2);
    ans.selfunified();
    int nl3 = double(clock() - t1) * 1000 / CLOCKS_PER_SEC;
    BitString::cnt_out_value(3);
    cout << "Executed Time:" << n2t << endl;
    
}

int BitString::addition_cnt = 0;
int BitString::shift_cnt = 0;
int BitString::recursive_call_cnt = 0;
int BitString::recursive_call_cnt2 = 0;
int main()
{
    BitString::cnt_init();
    vector<vector<string>> examples_X, examples_Y;
    int n=10;
    examples_X = make_test_samples(n);
    examples_Y = make_test_samples(n);
    
    for(int i = 0;i<=n-1;i++)
    {
        cout << "input length : 2 ^ " << i+1 << ":" << endl;

        vector<int> n_2_add_cnt, n_2_shfit_cnt, n_2_recursive_cnt, n2t;
        vector<int> nl3_add_cnt, nl3_shfit_cnt, nl3_recursive_cnt, nl3t;
        for(int j=0;j<10;j++)
        {
            string bit1 = examples_X[i][j];
            string bit2 = examples_Y[i][j];

            BitString::cnt_init();
            clock_t t1 = clock();
            BitString ans = bit1 * bit2;
            ans.selfunified();
            n2t.push_back(double(clock() - t1) * 1000 / CLOCKS_PER_SEC);
            BitString::cnt_out(n_2_add_cnt, n_2_shfit_cnt, n_2_recursive_cnt, 2);    
        }
        cout << "n_2_add_cnt : " << cal_avg(n_2_add_cnt) << endl;
        cout << "n_2_shift_cnt : " << cal_avg(n_2_shfit_cnt) << endl;
        cout << "n_2_recursive_cnt : " << cal_avg(n_2_recursive_cnt) << endl;
        cout << "time: " << cal_avg(n2t) << endl;
      
        for(int j=0;j<10;j++)
        {

            string bit1 = examples_X[i][j];
            string bit2 = examples_Y[i][j];

            BitString::cnt_init();
            clock_t t1 = clock();
            BitString ans = BitString::log3_Mult(bit1, bit2);
            ans.selfunified();
            nl3t.push_back(double(clock() - t1) * 1000 / CLOCKS_PER_SEC);
            BitString::cnt_out(nl3_add_cnt, nl3_shfit_cnt, nl3_recursive_cnt, 3);
        }
        cout << "nl3_add_cnt : " << cal_avg(nl3_add_cnt) << endl;
        cout << "nl3_shift_cnt : " << cal_avg(nl3_shfit_cnt) << endl;
        cout << "nl3_recursive_cnt : " << cal_avg(nl3_recursive_cnt) << endl;
        cout << "time: " << cal_avg(nl3t) << endl;
        
        
        
    }
    cout << endl;
    BitString s1("11001100");
    BitString s2("01010101");
    test(s1, s2);
}
