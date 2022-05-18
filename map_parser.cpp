#include<stdio.h>
#include<stdlib.h>
#include<fstream>
#include<iostream>
#include<unordered_map>
#include<cmath>
#include<unordered_set>
#include<iomanip>

/*
CODE SUBMITTED BY : SUBHAM GHOSH
ROLL NO. : 20CS10065
2ND YEAR UNDERGRADUATE STUDENT
DEPARTMENT OF COMPUTER SCIENCE AND ENGINEERING
IIT KHARAGPUR

IMPORTANT DATA : RADIUS OF EARTH = 6378.1 KILOMETER;
Please read the comments below before going through the code :
for k-closest nodes : the approach used first calculates distance of the given node to all other nodes and stores them in an array
                      it then heapifies the array. Call this the original heap. Now create another variable size new heap/proiority queue
                      Insert the min or root of the original heap into the priority queue
                      Now start the loop :
                                            until k-nodes are extracted from the new heap do the following
                                                    print and extract min from the priority queue
                                                    insert the children of this extracted min from the original heap into the queue

for search by name : As per question we have to search for the elements whose names have a substring matching exactly with the user given input
                     Hence by the language of the question it is implied that the search needs to be case-sensitive

for shortest path : Here is the approach -> (there is only one practical assumption made here : The Graph is Undirected)
                                            Create a priority queue
                                            Insert the starting node into the queue with distance = 0
                                            Now while queue is not empty do the following :
                                                dequeue most priority(nearest node)
                                                enqueue its neighbours into the queue with their cumulative distances
                                                if a neighbour is already present in the queue update its distance if new cumulative distance is lesser than the current distance
                                                if a neighbour had already been in the priority queue and dequeued before then do nothing to it

the above three functionalities are handled by there own classes
the UI is handled by the class Play_With_OpenStreetMap;
the XML file is parsed by the functions packaged inside the class My_Own_Freaking_OSM_Parser
User input along with exception_ handling is managed by the class Exception_Handled_Input_Taker,
the static boolean member variable exception_ stores if there was a exception_ during last user input

Moreover the function names and variable names are written in a way that explains their purpose.
*/

#define PI 3.1415926535897932384626433
#define MAX_LINE_LEN 256
using namespace std;

struct node {
    char* name;
    long double latitude;
    long double longitude;

    node() {
        this->name = NULL;
        this->latitude = 0;
        this->longitude = 0;
    }
}; // node elements

struct way {
    char* name;

    way() {
        name=NULL;
    }
}; // way elements

struct vertex { // unites the nodes with its adjacency list
    node element;
    unordered_set<long long unsigned int> adjacency_list; // adjacency list

    void add_vertex(long long unsigned int id) {
        adjacency_list.insert(id);
    }

};

struct dist_pair { // a struct datatype to specify a distance of a node from a given node
    long double distance;
    long long unsigned int point;

    dist_pair() {
        point = 0;
        distance = 0;
    }
};

long double calculate_distance(vertex& v1,vertex& v2) { // calculates crow-fly distance or geodesic distance between nodes
    long double phi1 = v1.element.latitude*PI/180;
    long double theta1 = v1.element.longitude*PI/180;
    long double phi2 = v2.element.latitude*PI/180;
    long double theta2 = v2.element.longitude*PI/180;

    long double dot_product = sin(phi1)*sin(phi2)+(cos(theta1-theta2))*cos(phi1)*cos(phi2);
    long double angle_between = acos(dot_product);
    return (6378.1*angle_between);
}

class Exception_Handled_Input_Taker {
    private :
    char buffer_[130];

    public :
    int exception_;

    Exception_Handled_Input_Taker() {
        exception_ = 0;
    }

    short int Get_Short_Int() {
        exception_ = 0;
        cin>>buffer_;
        short int output = 0;
        for(auto it : buffer_) {
            if(it=='\0') break;
            if(it<'0' || it>'9') {
                cout<<"\n<> You should have entered a number! The current function would be halted.\n";
                exception_ = 1;
                return 0;
            }
            else output = output*10 + it - '0';
        }
        return output;
    }

    long long unsigned int Get_Large_Unsigned_Number() {
        exception_ = 0;
        cin>>buffer_;
        long long unsigned int output = 0;
        for(auto it : buffer_) {
            if(it=='\0') break;
            if(it<'0' || it>'9') {
                cout<<"\n<> You should have entered a number! The current function would be halted.\n";
                exception_ = 1;
                return 0;
            }
            else output = output*10 + it - '0';
        }
        return output;
    }

    size_t Get_size_t_Number() {
        exception_ = 0;
        cin>>buffer_;
        size_t output = 0;
        for(auto it : buffer_) {
            if(it=='\0') break;
            if(it<'0' || it>'9') {
                cout<<"\n<> You should have entered a number! The current function would be halted.\n";
                exception_ = 1;
                return 0;
            }
            else output = output*10 + it - '0';
        }
        return output;
    }
};

class My_Own_Freaking_OSM_Parser {
    private :
    static inline long double extract_floating_point_number(const char str[]) { // extracts floating point numbers from a string
        long double fraction = 0;
        long double integer = 0;
        short int start = 0;
        short int stop = 0;

        while(str[start]<'0' || str[start]>'9') ++start;;
        stop = start;
        while(str[stop]=='.' || ((str[stop]>='0') && str[stop]<='9')) ++stop;;
        --stop;
        for(short int i=stop; str[i]!='.'; --i) fraction = (fraction/10) + (str[i]-'0')/(long double)10;;
        for(short int i=start; str[i]!='.'; ++i) integer = (integer*10) + (str[i]-'0');;
        return (integer + fraction);
    }

    static inline long long unsigned int extract_integer(const char str[]) { // extracts integer from a string
        long long unsigned int number = 0;
        short int index = 0;
        while(str[index]<'0' || str[index]>'9') ++index;
        while(str[index]>='0' && str[index]<='9') number = number*10 + str[index++] - '0' ;
        return number;
    }

    static inline char* extract_name_between_quotations(const char str[]) { // extract the substring under quotations in the string
        int i = 0;
        int length = 0;
        while(str[i]!='\"' && str[i]!='\0') ++i;

        if(str[i]=='\0') return NULL;
        ++i;

        while(str[i]!='\"' && str[i]!='\0') {
            ++i;
            ++length;
        }
        if(length==0) return NULL;
        ++length;

        if(str[i]=='\0') return NULL;
        --i;

        char* output = (char*) malloc(sizeof(char)*length);
        output[--length] = '\0';
        --length;
        while(length+1) {
            output[length] = str[i];
            --length;
            --i;
        }
        return output;
    }

    static inline int substring_check(const char org_str[],const char query_str[]) { // check substring matchings
        int b = 0;
        int j = 0;
        for(int i=0;org_str[i]!='\0' && query_str[j]!='\0';++i) {
            if(org_str[i] == query_str[j]) {
                ++j;
                b = 1;
            }
            else {
                j = 0;
                b = 0;
            }
        }
        return (b && (query_str[j]=='\0'));
    }

    public :
    static void get_counts(size_t& nodes,size_t& ways) { // quickly scans the initials of the text lines to get the count of nodes and ways
        //int i=0;
        ifstream map_file;
        map_file.sync_with_stdio(0);
        map_file.open("map.osm",ios::in);
        if(map_file.fail()) {
            cout<<"\n<> Big Error : Sorry the file does not exist! :(";
            exit(0);
        }
        char buffer[MAX_LINE_LEN];
        char node_query[] = "<n";
        char way_query[] = "<w";
        char stop_query[] = "<r";
        int state = 0;
        while(state!=3) {
            map_file>>buffer;
            switch(state) {
                case 0 :
                    state = 1;
                    nodes += substring_check(buffer,node_query);
                    break;
                case 1 :
                    nodes += substring_check(buffer,node_query);
                    if(substring_check(buffer,way_query)) {
                        ++ways;
                        state = 2;
                    }
                    break;
                case 2 :
                    ways += substring_check(buffer,way_query);
                    if(substring_check(buffer,stop_query)) {
                        state = 3;
                    }
                    break;
                default : continue;
            }
        }
        map_file.close();
    }

    static streampos extract_nodes(unordered_map<long long unsigned int,vertex>& graph,size_t& nodes) { // parses OSM file for nodes
        size_t index = 0;

        ifstream map_file;
        map_file.sync_with_stdio(0);
        map_file.open("map.osm",ios::in);

        char buffer[MAX_LINE_LEN];

        vertex place;
        long long unsigned int id;

        char node_query[] = "<no";
        char stop_node_query[] = "</no";
        char name_query[] = "k=\"name\"";
        char name_english_query[] = "k=\"name:en\"";
        char end_inline_query[] = "/>";
        char lat_reach_check_query[] = "lat=";

        while(index<nodes) {
            map_file>>buffer;
            if(substring_check(buffer,node_query)) {
                map_file>>buffer; //id
                id = extract_integer(buffer);
                while(!substring_check(buffer,lat_reach_check_query)) map_file>>buffer; // reach latitude attribute

                place.element.latitude = extract_floating_point_number(buffer);

                map_file>>buffer; //longitude

                place.element.longitude = extract_floating_point_number(buffer);

                if(!substring_check(buffer,end_inline_query)) {
                    while(!substring_check(buffer,stop_node_query)) {
                        map_file>>buffer;
                        if(substring_check(buffer,name_query)||substring_check(buffer,name_english_query)) {
                            map_file.getline(buffer,MAX_LINE_LEN,'>');
                            place.element.name = extract_name_between_quotations(buffer);
                        }
                    }
                }
                graph[id] = place;
                ++index;
                place.element.name=NULL;
            }
        }
        streampos pos_end_of_nodes = map_file.tellg();
        map_file.close();
        return pos_end_of_nodes;
    }

    static void extract_ways(unordered_map<long long unsigned int,way>& way_elements,size_t& ways,streampos& start_of_ways,unordered_map<long long unsigned int,vertex>& graph) {
        // parses OSM file for ways
        size_t index = 0;

        ifstream map_file;
        map_file.sync_with_stdio(0);
        map_file.open("map.osm",ios::in);

        char buffer[MAX_LINE_LEN];
        long long unsigned int last_vertex_id = 0;
        long long unsigned int current_vertex_id = 0;
        way a_way;
        long long unsigned int id;

        char way_query[] = "<wa";
        char stop_way_query[] = "</w";
        char name_query[] = "k=\"name\"";
        char name_english_query[] = "k=\"name:en\"";
        char node_reference_query[] = "<nd";

        map_file.seekg(start_of_ways);

        while(index<ways) {
            map_file>>buffer;
            if(substring_check(buffer,way_query)) {

                map_file>>buffer;
                id = extract_integer(buffer);

                while(!substring_check(buffer,stop_way_query)) {
                    map_file>>buffer;
                    if(substring_check(buffer,node_reference_query)) {
                        map_file>>buffer;
                        if(last_vertex_id==0) {
                            last_vertex_id = extract_integer(buffer);
                        }
                        else {
                            current_vertex_id = extract_integer(buffer);
                            graph[last_vertex_id].add_vertex(current_vertex_id);
                            graph[current_vertex_id].add_vertex(last_vertex_id);
                            last_vertex_id = current_vertex_id;
                        }
                    }
                    if(substring_check(buffer,name_query)||substring_check(buffer,name_english_query)) {
                        map_file.getline(buffer,MAX_LINE_LEN,'>');
                        a_way.name = extract_name_between_quotations(buffer);
                    }
                }
                way_elements[id] = a_way;
                a_way.name = NULL;
                last_vertex_id = 0;
                ++index;
            }
        }
        map_file.close();
    }
};

class Name_Search_Spider {
    private : // checks and finds a match if a parent string contains a substring same as the queries substring
    static inline int substring_check_ptr(const char* org_str,const char query_str[]) {
        if(org_str==NULL) return 0;
        int b = 0;
        int j = 0;
        for(int i=0;org_str[i]!='\0' && query_str[j]!='\0';++i) {
            if(org_str[i] == query_str[j]) {
                ++j;
                b = 1;
            }
            else {
                j = 0;
                b = 0;
            }
        }
        return (b && (query_str[j]=='\0'));
    }

    public : // it searches the user input keyword in the data of names of ways and nodes and prints the matches
    static void search_by_name(unordered_map<long long unsigned int,way>& way_elements,unordered_map<long long unsigned int,vertex>& graph) {
        char query[256];
        cout<<"\nEnter string to search : ";
        cin>>query;

        cout<<"\nMatches found in list of nodes";
        cout<<"\n------------------------------";
        for(auto iterator_var : graph) {
            if(substring_check_ptr(iterator_var.second.element.name,query)) {
                cout<<"\n ID : "<<iterator_var.first;
                cout<<",";
                if(iterator_var.first<100000000) cout<<" ";
                cout<<"\tlatitude : "<<iterator_var.second.element.longitude;
                cout<<", longitude : "<<iterator_var.second.element.latitude;
                cout<<", name : "<<iterator_var.second.element.name;
            }
        }
        cout<<"\n\nMatches found in list of ways";
        cout<<"\n-----------------------------";
        for(auto iterator_var : way_elements) {
            if(substring_check_ptr(iterator_var.second.name,query)) {
                cout<<"\n ID : "<<iterator_var.first;
                cout<<",";
                if(iterator_var.first<100000000) cout<<" ";
                cout<<"\tname : "<<iterator_var.second.name;
            }
        }

    }
};

class K_Closest_Nodes {
    private : // this private functions implement heap data structure
    static void heapify(dist_pair arr[],size_t n) {
        size_t i = n-1;
        size_t j;
        dist_pair temp;
        while(1) {
            j = i;
            while(1) {
                if(2*j+2<n) {
                    if(arr[2*j+1].distance<arr[2*j+2].distance) {
                        if(arr[j].distance<arr[2*j+1].distance) break;
                        else {
                            temp = arr[j];
                            arr[j] = arr[2*j+1];
                            arr[2*j+1] = temp;
                            j = 2*j+1;
                        }
                    }
                    else {
                        if(arr[j].distance<arr[2*j+2].distance) break;
                        else {
                            temp = arr[j];
                            arr[j] = arr[2*j+2];
                            arr[2*j+2] = temp;
                            j = 2*j+2;
                        }
                    }
                }
                else if(2*j+1<n) {
                    if(arr[j].distance<arr[2*j+1].distance) break;
                    else {
                        temp = arr[j];
                        arr[j] = arr[2*j+1];
                        arr[2*j+1] = temp;
                        j = 2*j+1;
                    }
                }
                else break;
            }
            if(i==0) break;
            --i;
        }

    }

    static void replace(size_t darr[],dist_pair arr[],size_t dsz,size_t eli) {
        darr[0] = eli;
        size_t temp;
        size_t j=0;
        while(1) {
            if(2*j+2<dsz) {
                if(arr[darr[2*j+1]].distance<arr[darr[2*j+2]].distance) {
                    if(arr[darr[j]].distance<arr[darr[2*j+1]].distance) break;
                    else {
                        temp = darr[j];
                        darr[j] = darr[2*j+1];
                        darr[2*j+1] = temp;
                        j = 2*j+1;
                    }
                }
                else {
                    if(arr[darr[j]].distance<arr[darr[2*j+2]].distance) break;
                    else {
                        temp = darr[j];
                        darr[j] = darr[2*j+2];
                        darr[2*j+2] = temp;
                        j = 2*j+2;
                    }
                }
            }
            else if(2*j+1<dsz) {
                if(arr[darr[j]].distance<arr[darr[2*j+1]].distance) break;
                else {
                    temp = darr[j];
                    darr[j] = darr[2*j+1];
                    darr[2*j+1] = temp;
                    j = 2*j+1;
                }
            }
            else break;
        }
    }

    static void add(size_t darr[],dist_pair arr[],size_t& dsz,size_t eli) {
        darr[dsz] = eli;
        size_t j = dsz;
        size_t temp;
        ++dsz;
        while(j) {
            if(arr[darr[j]].distance<arr[darr[(j-1)/2]].distance) {
                temp = darr[j];
                darr[j] = darr[(j-1)/2];
                darr[(j-1)/2] = temp;
                j = (j-1)/2;
            }
            else break;
        }
    }

    static void popout(size_t darr[],dist_pair arr[],size_t& dsz) {
        darr[0] = darr[dsz-1];
        --dsz ;
        size_t temp;
        size_t j=0;
        while(1) {
            if(2*j+2<dsz) {
                if(arr[darr[2*j+1]].distance<arr[darr[2*j+2]].distance) {
                    if(arr[darr[j]].distance<arr[darr[2*j+1]].distance) break;
                    else {
                        temp = darr[j];
                        darr[j] = darr[2*j+1];
                        darr[2*j+1] = temp;
                        j = 2*j+1;
                    }
                }
                else {
                    if(arr[darr[j]].distance<arr[darr[2*j+2]].distance) break;
                    else {
                        temp = darr[j];
                        darr[j] = darr[2*j+2];
                        darr[2*j+2] = temp;
                        j = 2*j+2;
                    }
                }
            }
            else if(2*j+1<dsz) {
                if(arr[darr[j]].distance<arr[darr[2*j+1]].distance) break;
                else {
                    temp = darr[j];
                    darr[j] = darr[2*j+1];
                    darr[2*j+1] = temp;
                    j = 2*j+1;
                }
            }
            else break;
        }
    }

    public : // the below algorithm runs in O(N + K*LOG(K)) time where N is the number of nodes and K is given by user
    // as a matter of fact this is the most efficient possible way to find k-nearest nodes
    static void find_k_closest_nodes(unordered_map<long long unsigned int,vertex>& graph,size_t nodes) {
        Exception_Handled_Input_Taker user_input;
        cout<<"\n";

        long long unsigned int id;
        cout<<"\nEnter the ID of the node : ";
        id = user_input.Get_Large_Unsigned_Number();
        if(user_input.exception_) return ;
        if(graph.find(id)==graph.end()) {
            cout<<"<> Sorry the ID is absent in the database!";
            return ;
        }

        size_t k;
        cout<<"How many closest nodes you want to find ? k = ";
        k = user_input.Get_size_t_Number();
        if(user_input.exception_) return ;
        if(k>=nodes) {
            cout<<"<> Sorry k = "<<k<<" should be less than the number of nodes in the area/graph except the queried point!"<<" (i.e. "<<nodes-1<<")";
            return ;
        }

        size_t j=0;
        dist_pair arr[nodes-1];
        for(auto iterator_var : graph) {
            if(iterator_var.first!=id) {
                arr[j].distance = calculate_distance(graph[id],graph[iterator_var.first]);
                arr[j].point = iterator_var.first;
                ++j;
            }
        }

        j = 0;
        size_t darr[k];
        size_t dsz=1;

        size_t root;
        darr[0] = 0;
        size_t sz = nodes-1;
        heapify(arr,sz);

        for(size_t i=0;i<k;++i) {
            root = darr[0];
            cout<<"ID : "<<arr[root].point<<",";
            if(arr[root].point<1000000000) cout<<"  ";
            if(graph[arr[root].point].element.name!=NULL) cout<<"\tdistance = "<<arr[root].distance<<" K.M.,\tname : "<<graph[arr[root].point].element.name;
            else cout<<"\tdistance = "<<arr[root].distance<<" K.M.,\tname : "<<"(null)";
            cout<<"\n";

            if(2*root+1<sz) replace(darr,arr,dsz,2*root+1);
            else {
                popout(darr,arr,dsz);
            }

            if(2*root+2<sz) add(darr,arr,dsz,2*root+2);
        }
    }
};

class Shortest_Path_Finder {
    private : // this private functions are used for implementing priority queue based on heap data structure
    static void add_to_queue(dist_pair arr[],size_t& dsz,dist_pair eli,unordered_map<long long unsigned int,size_t>& heap_loc) {
        arr[dsz] = eli;
        heap_loc[eli.point] = dsz;

        size_t j = dsz;
        dist_pair temp;
        ++dsz;

        while(j) {
            if(arr[j].distance<arr[(j-1)/2].distance) {
                heap_loc[arr[j].point] = (j-1)/2;
                heap_loc[arr[(j-1)/2].point] = j;
                temp = arr[j];
                arr[j] = arr[(j-1)/2];
                arr[(j-1)/2] = temp;
                j = (j-1)/2;
            }
            else break;
        }
    }

    static dist_pair pop_queue(dist_pair arr[],size_t& dsz,unordered_map<long long unsigned int,size_t>& heap_loc) {
        dist_pair temp = arr[0];
        arr[0] = arr[dsz-1];
        heap_loc[arr[0].point] = 0;
        arr[dsz-1] = temp;
        --dsz ;
        size_t j=0;
        while(1) {
            if(2*j+2<dsz) {
                if(arr[2*j+1].distance<arr[2*j+2].distance) {
                    if(arr[j].distance<arr[2*j+1].distance) break;
                    else {
                        heap_loc[arr[j].point] = 2*j+1;
                        heap_loc[arr[2*j+1].point] = j;
                        temp = arr[j];
                        arr[j] = arr[2*j+1];
                        arr[2*j+1] = temp;
                        j = 2*j+1;
                    }
                }
                else {
                    if(arr[j].distance<arr[2*j+2].distance) break;
                    else {
                        heap_loc[arr[j].point] = 2*j+2;
                        heap_loc[arr[2*j+2].point] = j;
                        temp = arr[j];
                        arr[j] = arr[2*j+2];
                        arr[2*j+2] = temp;
                        j = 2*j+2;
                    }
                }
            }
            else if(2*j+1<dsz) {
                if(arr[j].distance<arr[2*j+1].distance) break;
                else {
                    heap_loc[arr[j].point] = 2*j+1;
                    heap_loc[arr[2*j+1].point] = j;
                    temp = arr[j];
                    arr[j] = arr[2*j+1];
                    arr[2*j+1] = temp;
                    j = 2*j+1;
                }
            }
            else break;
        }
        return arr[dsz];
    }

    static void update_queue(dist_pair arr[],size_t change_loc,long double new_distance,unordered_map<long long unsigned int,size_t>& heap_loc) {
        arr[change_loc].distance = new_distance;
        size_t j = change_loc;
        dist_pair temp;

        while(j) {
            if(arr[j].distance<arr[(j-1)/2].distance) {
                heap_loc[arr[j].point] = (j-1)/2;
                heap_loc[arr[(j-1)/2].point] = j;
                temp = arr[j];
                arr[j] = arr[(j-1)/2];
                arr[(j-1)/2] = temp;
                j = (j-1)/2;
            }
            else break;
        }
    }

    public :
    static void find_shortest_path(unordered_map<long long unsigned int,vertex>&graph,size_t nodes) {
        // runs the efficient version of djikstra algorithm as it takes much less time than the normal implementation
        // for more information read the initial comments at the start if the program
        Exception_Handled_Input_Taker user_input;
        cout<<"\n";
        long long unsigned int id1,id2;

        cout<<"\nEnter ID of the starting node : ";
        id2 = user_input.Get_Large_Unsigned_Number(); // starting node
        if(user_input.exception_) return ;
        if(graph.find(id2)==graph.end()) {
            cout<<"<> Sorry the ID is absent in the database!";
            return ;
        }

        cout<<"Enter ID of the ending node   : ";
        id1 = user_input.Get_Large_Unsigned_Number(); // ending node
        if(user_input.exception_) return ;
        if(graph.find(id1)==graph.end()) {
            cout<<"<> Sorry the ID is absent in the database!";
            return ;
        }

        dist_pair dist_queue[nodes];
        size_t dsz = 1;

        dist_pair temp;
        dist_pair root;
        unordered_map<long long unsigned int,long long unsigned int> intermediary; // contains the intermediate neighbour of a node in the tree formed from djikstra algorithm
        unordered_map<long long unsigned int,size_t> heap_loc; // stores the real-time location of the node in the priority queue
        unordered_map<long long unsigned int,long double> shortest_dist; // stores the magnitude of shortest distance of a node

        dist_queue[0].distance = 0;
        dist_queue[0].point = id1;
        intermediary[id1] = 0;
        heap_loc[id1] = 0;
        short int found = 0;
        long double temp_dist;

        while(dsz) {

            root = pop_queue(dist_queue,dsz,heap_loc); //extract out current nearest node

            shortest_dist[root.point] = root.distance;
            if(root.point==id2) {
                found = 1;
                break;
            }
            for(auto iterator_var : graph[root.point].adjacency_list) {
                if(heap_loc.find(iterator_var)==heap_loc.end()) { // adding node to queue if not already added
                    temp.distance = calculate_distance(graph[root.point],graph[iterator_var]) + root.distance;
                    temp.point = iterator_var;
                    intermediary[temp.point] = root.point;
                    add_to_queue(dist_queue,dsz,temp,heap_loc);
                }
                else if(heap_loc[iterator_var]!=0) { // updating the distance of a node currently in queue if smaller than current distance
                    temp_dist = calculate_distance(graph[root.point],graph[iterator_var]);
                    if((root.distance + temp_dist) < dist_queue[heap_loc[iterator_var]].distance) {
                        update_queue(dist_queue,heap_loc[iterator_var],root.distance + temp_dist,heap_loc);
                        intermediary[iterator_var] = root.point;
                    }
                }
            }
        }

        if(found) {
            cout<<"\n<> Distance of Shortest Path : "<<shortest_dist[id2]<<" K.M.";
            cout<<"\n\n<> The Shortest Path is : ";
            cout<<id2;
            while(intermediary[id2]) { // prints the path from start to end
                id2 = intermediary[id2];
                cout<<" -> "<<id2;
            }
        }
        else {
            cout<<"\n<> Sorry no path exists between these nodes via the way elements currently parsed in the file!";
        }
        /*
                one might observe that the id1 and id2 are reversed during taking input
                Although this does not affect the result as graph is undirected yet the logic behind such
                a reversal is because of the way the intermediates are stored to a particular vertex.
                Because intermediates can be retrived backwards hence at the end two reversals give the forward path from
                start node to end node
        */
    }
};

class Play_With_OpenStreetMap{
    private :
    size_t nodes; // count of nodes
    size_t ways; // count of ways
    unordered_map<long long unsigned int,vertex> graph; // the data about nodes
    unordered_map<long long unsigned int,way> way_elements; // the data about way elements
    short int option; // user choice

    public :
    int have_fun() {
        cout<<"\n!! Do make sure that the OSM file is named \"map.osm\" and is present in the current directory. :)\n";
        nodes = 0;
        ways = 0;
        My_Own_Freaking_OSM_Parser::get_counts(nodes,ways);
        cout<<"\nOSM File Parsing Done! There are "<<nodes<<" nodes and "<<ways<<" ways\n";
        Exception_Handled_Input_Taker user_input;
        streampos start_of_ways = My_Own_Freaking_OSM_Parser::extract_nodes(graph,nodes);
        My_Own_Freaking_OSM_Parser::extract_ways(way_elements,ways,start_of_ways,graph);

        while(1) {
            cout<<"\n"<<"Case [1] : Search Nodes or Ways by their Name\n";
            cout<<"Case [2] : Find k-closest Nodes to a given Node\n";
            cout<<"Case [3] : Find and Print Shortest Path between two Nodes\n";
            cout<<"Case [0] : Exit\n";
            cout<<"Enter Your Option : ";
            option = user_input.Get_Short_Int();
            switch(option) {
                case 1 : Name_Search_Spider::search_by_name(way_elements,graph); break;
                case 2 : K_Closest_Nodes::find_k_closest_nodes(graph,nodes); break;
                case 3 : Shortest_Path_Finder::find_shortest_path(graph,nodes); break;
                case 0 : return 0;
                default : cout<<"\n<> You should have entered within the option numbers provided!";
            }
            cout<<"\n";
        }
    }
};

int main() {
    // the class object encapsulating the map-data of graphs and edges and the associated functions to perform the given operations
    cout<<setprecision(8)<<fixed;
    Play_With_OpenStreetMap kidding_object;
    kidding_object.have_fun(); // starts and runs the command line User Interface
    return 0;
}
