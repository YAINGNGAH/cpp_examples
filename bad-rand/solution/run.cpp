#include <bits/stdc++.h>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/matrix.hpp>

//"NLXGI4NoAp"
//val = ((state[0] * 1103515245) + 12345) & 0x7fffffff; in code idk
// int r[MAX]; // internet
//  int i;
//
//  r[0] = seed;
//  for (i=1; i<31; i++) {
//    r[i] = (16807LL * r[i-1]) % 2147483647;
//    if (r[i] < 0) {
//      r[i] += 2147483647;
//    }
//  }
//  for (i=31; i<34; i++) {
//    r[i] = r[i-31];
//  }
//  for (i=34; i<344; i++) {
//    r[i] = r[i-31] + r[i-3];
//  }
//  for (i=344; i<MAX; i++) {
//    r[i] = r[i-31] + r[i-3];
//    printf("%d\n", ((unsigned int)r[i]) >> 1);
//  }
//
const int MAX = 354;
const std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
void GenString(char *str, int length) {
    for (int i = 0; i < length; ++i) {
        str[i] = alphabet[rand() % alphabet.size()];
    }
}



std::string gen(int sd) {
    int seed = sd;
    unsigned int r[MAX];
    std::string s = "";
    int i;

    r[0] = seed;
    for (i=1; i<31; i++) {
        r[i] = (16807LL * r[i-1]) % 2147483647;
        if (r[i] < 0) {
            r[i] += 2147483647;
        }
    } //first step is reverse string to this shit.
    for (i=31; i<34; i++) {
        r[i] = r[i-31];
    }
    for (i=34; i<344; i++) {
        r[i] = r[i-31] + r[i-3];
    }
    for (i=344; i<MAX; i++) {
        r[i] = r[i-31] + r[i-3];
        s.push_back(alphabet[(r[i] >> 1) % alphabet.size()]);
    }
    return s;
}

int qrand(int sd){
    int seed = sd;
    unsigned int r[MAX];
    std::string s = "";
    int i;

    r[0] = seed;
    for (i=1; i<31; i++) {
        r[i] = (16807LL * r[i-1]) % INT32_MAX;
    } //first step is reverse string to this shit.
    for (i=31; i<34; i++) {
        r[i] = r[i-31];
    }
    for (i=34; i<344; i++) {
        r[i] = r[i-31] + r[i-3];
    }
    for (i=344; i<MAX; i++) {
        r[i] = r[i-31] + r[i-3];
    }
    return (r[344] >> 1);
}


//fuck it попробую другое
//((state[0] * 1103515245U) + 12345U) & 0x7fffffff
//int state;
//    std::string shit = "NLXGI4NoAp";
//    bool check;
//    std::string line("..........");
//    int l = 0;
//    for (unsigned int i = 0; i < UINT_MAX; ++i) {
//        state = i;
//        check = true;
//        if (i%(INT_MAX/10) == 0 && l<=9){
//            line[l] = '*';
//            cout << line << "\n";
//            l++;
//        }
//        for (int j = 0; j < 10; ++j) {
//            state = ((state * 1103515245U) + 12345U) & 0x7fffffff;
//            int ostat = ((unsigned)state/65536)%32768;
//            if (alphabet[ostat%alphabet.size()] != shit[j]){
//                check = false;
//                break;
//            }
//        }
//        if (check){
//            cout << i << "\n";
//            for (int j = 0; j < 10; ++j) {
//                state = ((state * 1103515245U) + 12345U) & 0x7fffffff;
//                int ostat = ((unsigned)state/65536)%32768;
//                cout << alphabet[ostat%alphabet.size()];
//            }
//            cout <<"\n";
//        }
//    }

using namespace boost::numeric::ublas;
template<class E>
matrix<E> pow(matrix<E> m, unsigned int p){
    if (p == 1){
        return m;
    }
    if (p%2 == 0){
        auto m1 = pow(m, p/2);
        return prod(m1,m1);
    }else{
        return prod(pow(m,p-1), m);
    }
}

int brute1(){
    //феерия брутфорса
    std::string shit = "NLXGI4NoAp";
    int n = 31;
    matrix<int> m(n, n);
    for (unsigned i = 0; i < n-1; ++ i)
        m (i+1, i) = 1;
    m(0, n-1) = 1;
    m(n-3, n-1) = 1;
    auto proc = pow(m, 1e8 + 310);
    vector<uint32_t> init(n, 1);
    std::vector<unsigned int> a;
    std::string krutilka = "|/-\\";
    time_t cur;
    time(&cur);
    //если что я не шизофреник, и распаллелил на несколько ядер впоследствии
    for (uint32_t i = 1; i < UINT32_MAX ; ++i) {
        if (time(nullptr) - cur > 0){
            time(&cur);
            std::cout << "\rCompleted: " << (double)i/(UINT32_MAX/100) << "% " <<krutilka[i%4] << std::flush;
        }
        init(0) = i;
        for (int j = 1; j < 31; ++j) {
            init(j) = (16807LL * init[j-1]) % INT32_MAX;
        }
        std::rotate(init.begin(), init.begin() + 3, init.end());
        vector<uint32_t> res = prod(init, proc);
        bool check = true;
        for (int j = 0; j < 10; ++j) {
            if (alphabet[(res[j + 21]>>1)%alphabet.size()] != shit[j]){
                check = false;
                break;
            }
        }
        if (check){
            a.push_back(i);
            std::cout << "\nFind : " << i << std::endl;
        }
    }
    for (int i = 0; i < a.size(); ++i) {
        std::cout << "\n\n\nFUCK YOU, SEED: " << a[i] << std::endl;
    }

    return 0;
}

int main(){
    brute1();
    return 0;
}

//Find : 1543750888
//Completed: 35.9441%