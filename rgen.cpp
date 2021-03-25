#include <iostream>
#include <cmath>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <limits>
#include <algorithm>

using namespace std;

int randint(int min_val, int max_val){
    std::ifstream urandom("/dev/urandom");
    unsigned int num;
    urandom.read((char *) &num, sizeof(num));
    urandom.close();

    if (min_val==max_val){
        return min_val;
    }else{
        int rand_num = (num%(max_val-min_val+1));
        rand_num = rand_num+min_val;
        return rand_num;
    }
}

bool are_zero_line_segment(vector<int> point1, vector<int> point2){
    int x1 = point1[0], y1 = point1[1], x2 = point2[0], y2 = point2[1];
    return x1==x2 and y1==y2;
}

bool is_intersection_on_line_segment(vector<int>point_l1, vector<int>point_l2,vector<double>point_i){
    double xi=point_i[0];
    double yi=point_i[1];
    //keep x1<=x2, y1<=y2
    vector<int> x_range, y_range;
    x_range.push_back(point_l1[0]);
    x_range.push_back(point_l2[0]);
    y_range.push_back(point_l1[1]);
    y_range.push_back(point_l2[1]);

    sort(x_range.begin(), x_range.end());
    sort(y_range.begin(), y_range.end());

    if ((x_range[0]<=xi)&&(xi<=x_range[1])&&(y_range[0]<=yi)&&(yi<=y_range[1])){
        return true;
    }else{
        return false;
    }
}

bool are_line_segment_overlap(vector<int> point1, vector<int> point2, vector<int> point3, vector<int> point4){
    int x1, y1, x2, y2, x3, y3, x4, y4;

    // k do not exist; keep y1 <= y2; y3 <= y4
    if (point1[0]==point2[0] && point3[0]==point4[0] && point1[0]==point3[0]){
        if (point1[1]<point2[1]){
            y1 = point1[1], y2 = point2[1];
        }else{
            y1 = point2[1], y2 = point1[1];
        }
        if (point3[1]<point4[1]){
            y3 = point3[1], y4 = point4[1];
        }else{
            y3 = point4[1], y4 = point3[1];
        }

        if ((y1<=y3&&y3<y2)||(y1<y4&&y4<=y2)||(y3<=y1&&y1<y4)||(y3<y2&&y2<=y4)){
            return true;
        }
    }

    // if k exists; keep x1 <= x2, x3 <= x4
    if (point1[0] != point2[0] and point3[0] != point4[0]){
        if (point1[0]< point2[0]){
            x1=point1[0], y1=point1[1], x2=point2[0], y2=point2[1];
        }else{
            x1=point2[0], y1=point2[1], x2=point1[0], y2=point1[1];
        }
        if (point3[0]<point4[0]){
            x3=point3[0], y3=point3[1], x4=point4[0], y4=point4[1];
        }else{
            x3=point4[0], y3=point4[1], x4=point3[0], y4=point3[1];
        }
        double k1=(y2-y1);
        k1 = k1/(x2-x1);
        double k2=(y4-y3);
        k2 = k2/(x4-x3);
        double b1=(x2*y1-x1*y2);
        b1 = b1/(x2-x1);
        double b2=(x4*y3-x3*y4);
        b2 = b2/(x4-x3);
        if (k1 == k2 && b1 == b2){
            if((x1<=x3&&x3<x2)||(x1<x4&&x4<=x2)||(x3<=x1&&x1<x4)||(x3<x2&&x2<=x4)){
                return true;
            }
        }
    }
    return false;
}

bool are_line_segment_intersect(vector<int> point1, vector<int> point2, vector<int> point3, vector<int> point4){
    int x1=point1[0], y1=point1[1], x2=point2[0], y2=point2[1];
    int x3=point3[0], y3=point3[1], x4=point4[0], y4=point4[1];

    if (point1[0]==point2[0] and point3[0]!=point4[0]){
        double k2 = (y4-y3);
        k2 = k2/(x4-x3);
        double b2 = (x4*y3-x3*y4);
        b2 = b2/(x4-x3);

        double y_ist = k2*x1 + b2;
        vector<double> intersection;
        intersection.push_back(x1);
        intersection.push_back(y_ist);
        if ((is_intersection_on_line_segment(point1,point2,intersection)) and (is_intersection_on_line_segment(point3,point4,intersection))){
            return true;
        }
    }
    if (point3[0]==point4[0] and point1[0]!=point2[0]){
        double k1=(y2-y1);
        k1=k1/(x2-x1);
        double b1=(x2*y1-x1*y2);
        b1=b1/(x2-x1);

        double y_ist = k1*x3 + b1;
        vector<double> intersection;
        intersection.push_back(x3);
        intersection.push_back(y_ist);
        if ((is_intersection_on_line_segment(point3,point4,intersection)) and (is_intersection_on_line_segment(point1,point2,intersection))){
            return true;
        }
    }
    if (point1[0]!=point2[0] and point3[0]!=point4[0]){
        double k1=(y2-y1);
        k1=k1/(x2-x1);
        double b1=(x2*y1-x1*y2);
        b1=b1/(x2-x1);
        double k2=(y4-y3);
        k2=k2/(x4-x3);
        double b2=(x4*y3-x3*y4);
        b2=b2/(x4-x3);

        if (k1!=k2){
            double x_ist=(b2-b1);
            x_ist = x_ist/(k1-k2);
            double y_ist=(k1*b2-k2*b1);
            y_ist = y_ist/(k1-k2);

            vector<double> intersection;
            intersection.push_back(x_ist);
            intersection.push_back(y_ist);

            if ((is_intersection_on_line_segment(point1,point2,intersection)) and (is_intersection_on_line_segment(point3,point4,intersection))){
                return true;
            }
        }
    }
    return false;
}


int main(int argc, char *argv[]){
    int option;

    string s_string, n_string, l_string, c_string;
    int s = 10;
    int n = 5;
    int l = 5;
    int c = 20;

    while((option=getopt(argc,argv,"s:n:l:c:")) != -1){
        switch (option) {
            case 's':
                s_string=optarg;
                s = atoi(s_string.c_str());  //convert string(C-string) to integer
                break;

            case 'n':
                n_string=optarg;
                n = atoi(n_string.c_str());
                break;

            case 'l':
                l_string=optarg;
                l = atoi(l_string.c_str());
                break;

            case 'c':
                c_string=optarg;
                c = atoi(c_string.c_str());
                break;

        }
    }

    vector<string> streetList;
    while(1){
        int st_temp = 1;
        int s_num = randint(2,s); // # of the streets
        int l_num = randint(5,l); // # of seconds to wait
        vector< vector< vector<int> > > graph;

        // make streetList, graph

        while (st_temp <= s_num) {
            int n_num = randint(1, n); // # of line segments in a street
            vector<vector<int>> street;

            int n_temp = 1;
            // make valid street
            // n+1 points only hold n line segments
            while (n_temp <= n_num + 1) {
                vector<int> point_temp;

                point_temp.push_back(randint(-c, c));
                point_temp.push_back(randint(-c, c));

                if (!street.empty()) {
                    int flag1 = 0, flag2 = 0, flag3 = 0, flag4 = 0;
                    int count = 0;
                    while (flag1 == 0 || flag2 == 0 || flag3 == 0 || flag4 == 0) {
                        count += 1;
                        if (count > 25) {
                            cerr << "Error: failed to generate valid input for 25 simultaneous attempts. \n";
                            exit(1);
                        }
                        point_temp[0] = randint(-c, c);
                        point_temp[1] = randint(-c, c);

                        // check zero-length line segment
                        if (!are_zero_line_segment(point_temp, street.back())) {
                            flag1 = 1;
                        } else {
                            flag1 = 0;
                        }

                        // check overlap within the street
                        int flag22 = 0;
                        if (street.size() > 1) {
                            for (unsigned int i = 0; i < street.size() - 1; i++) {
                                if (are_line_segment_overlap(street[i], street[i + 1], street.back(), point_temp)) {
                                    flag22 = 1;
                                }
                            }
                        }
                        if (flag22 == 1) {
                            flag2 = 0;
                        } else {
                            flag2 = 1;
                        }

                        // check overlap with other street
                        int flag33 = 1;
                        if (graph.size() > 1) {
                            for (unsigned int i = 0; i < graph.size() - 1; i++) {
                                for (unsigned int j = 0; j < graph[i].size() - 1; j++) {
                                    if (are_line_segment_overlap(graph[i][j], graph[i][j + 1], street.back(),
                                                                 point_temp)) {
                                        flag33 = 0;
                                    }
                                }
                            }
                        }
                        if (flag33 != 0) {
                            flag3 = 1;
                        } else {
                            flag3 = 0;
                        }

                        // check intersection within the street (maybe flag44 is redundant)
                        int flag44 = 0;

                        if (street.size() > 2) {
                            for (unsigned int i = 0; i < street.size() - 2; i++) {
                                if (are_line_segment_intersect(street[i], street[i + 1], street.back(),
                                                               point_temp)) {
                                    flag44 = 1;
                                }
                            }
                        }
                        if (flag44 == 1) {
                            flag4 = 0;
                        } else {
                            flag4 = 1;
                        }
                    }
                }
                street.push_back(point_temp);
                n_temp++;
            }

            graph.push_back(street);
            st_temp++;
        }

        // print a command
        for(unsigned int i=0; i<graph.size(); i++){
            streetList.push_back("street " + to_string(i+1));
            cout << "a \"street " << i+1 << "\"";
            for (unsigned int j=0; j<graph[i].size(); j++){
                cout << " (" << graph[i][j][0] << "," << graph[i][j][1] << ")";
            }
            cout << endl;
        }

        // print g command
        cout << "g" << endl;

        // wait some second (for linux)
        sleep(l_num);

        // print r command
        if (!streetList.empty()){
            for (unsigned int i=0; i<streetList.size();i++){
                cout << "r \"" << streetList[i] << "\"" << endl;
            }
        }
        streetList.clear();
    }
    return 0;
}
