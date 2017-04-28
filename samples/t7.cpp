#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <limits>
#include <algorithm>

template <typename T>
class Stat {
public:
    T max;
    T min;
    int num;
    float mean;
    std::map<T,int> freq_dist;
    const T precision;
    int outlier_count;
    float outlier_fraction;

    Stat(T p=1, int oc=100, float of=0.1) :
            max(std::numeric_limits<T>::lowest()),
            min(std::numeric_limits<T>::max()),
            num(0), mean(0), freq_dist(), precision(p),
            outlier_count(oc), outlier_fraction(of)
            {std::cerr<<"construcor called"<<__func__<<"\n";}

    T bucket (T x) {
        return precision * int(x/precision);
    }

    void add(T record) {
        max = std::max(max, record);
        min = std::min(min, record);
        const int n = num;
        mean = float(mean)*(float(n)/(n+1)) + float(record)/(float(n)+1);
        num+=1;
        freq_dist[bucket(record)]+=1;
    }
};

template <typename T>
class VectorStat {
public:
    std::map<int,Stat<T>> stats;
    const T precision;
    int outlier_count;
    float outlier_fraction;

    VectorStat(T p=1, int oc=100, float of=0.1) :
        precision(p), outlier_count(oc), outlier_fraction(of)
        {} ;
    // {std::cerr<<"construcor called"<<__func__<<"\n";}

    void add(std::vector<T> records) {
        int i = 0;
        for(auto record: records){
            if( stats.find(i)==stats.end() ) {
                stats.emplace(i, Stat<T>(precision, outlier_count, outlier_fraction));
            }
            stats[i].add(record);

            i++;
        }
    }

};
template <typename T, typename U>
std::ostream& to_string(std::ostream& os, std::pair<T,U>p) {
    return os << "<" << p.first <<","<< p.second << ">";
}

template <typename T>
std::ostream& to_string(std::ostream& os, T begin, T end) {
    os << "[";
    for(auto it=begin; it!=end; ++it) {
        to_string(os, *it) << ",";
    }
    return os << "]";
}

template <typename T>
std::ostream& operator<<(std::ostream& s, Stat<T> stat) {
    std::vector<std::pair<T,int>> outliers;
    int cumfreq = 0;
    int cumcount = 0;
    for(auto& p : stat.freq_dist) {
        cumfreq += p.second;
        cumcount += 1;
        if( (cumcount<stat.outlier_count) && (cumfreq<stat.num*stat.outlier_fraction) )
            outliers.push_back(p);
        else
            break;
    }
    s<< "max:" << stat.max << " "
     << "min:" << stat.min << " "
     << "num:" << stat.num << " "
     << "mean:" << stat.mean << " ";
    s << "outliers:";
    return to_string(s, outliers.begin(), outliers.end());
}

template <typename T>
std::ostream& operator<<(std::ostream& os, VectorStat<T> vector_stat) {
    // os << vector_stat.stats.size() << "\n";
    os << "{\n";
    for(auto& it : vector_stat.stats) {
        os << it.first;
        os << " : ";
        os << it.second;
        os << "\n";
    }
    os << "}";
    return os;
}

int main(){

    std::vector<float> n1 {-100, 5.32, 24};
    std::vector<std::vector<float>> n2
        {{2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},
        {2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},
        {2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},
        {2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},
        {2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},
        {2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},
        {2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25},{2, 6.32, 25}};

    Stat<float> s1;
    std::cout << s1 << "\n";

    s1.add(1);
    std::cout << s1 << "\n";

    s1.add(2);
    std::cout << s1 << "\n";


    VectorStat<float> s2{};
    s2.add(n1);
    for (auto n: n2) {
        s2.add(n);
    }

    std::cout << s2 << "\n";

}
