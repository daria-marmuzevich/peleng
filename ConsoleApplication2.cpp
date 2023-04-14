#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <queue>
#include <thread>
using namespace std::chrono_literals;
using namespace std;

class MeteoStation {

public:
    vector<pair<string, int>> queue;
    deque<string> speci_msgs;
    deque<string> metar_msgs;
    deque<string> kn01_msgs;
    deque<string> awos_msgs;
    int state = 1;
    string speci_file_name = "Send/speci.txt";
    string metar_file_name = "Send/metar.txt";
    string kn01_file_name = "Send/kn01.txt";
    string awos_file_name = "Send/awos.txt";


    bool isCorrecrt(string line, int pos2, int pos3) {

        char g[4];
        line.copy(g, 4, 6);
        union {
            char g_reverse[4];
            int len;
        };
        reverse_copy(g, g + 4, g_reverse);
      
        return pos3 - pos2 - 1 == len;    }

    int find(istream& file, char b) {
        int fileSize = file.tellg();

        bool found = false;
        while (!found && fileSize > 0)
        {

            file.seekg(--fileSize);
            char c = file.get();
            if (uint16_t(c) == b) {
                return fileSize;
            }
        }
        return -1;
           
    }
 
    deque<string>* find_deq(int priority) {
        switch (priority) {
        case 1:
            return &speci_msgs;
        case 2:
            return &metar_msgs;
        case 3:
            return &kn01_msgs;
        case 4:
            return &awos_msgs;
        default:
            return nullptr;

        }
    }


    void setToLastLine(ifstream& file, int priority) {
        if (!file.is_open())
        {
            std::cout << "Cannot open file." << std::endl;
        }

        //переместить указатель чтения в конец файла
        file.seekg(0, std::ios::end);

        // получить позицию указателя чтения (то есть размер файла)
        int fileSize = file.tellg();
        int fsize = fileSize;
   
        bool found1 = false;
        bool found = false;
        int len = 0;
       

        // если нашли начало строки, то переместить указатель чтения на эту позицию

        int pos3 = find(file, 3);
        int pos2 = find(file, 2);
        int pos_slash = find(file, '/');
        int pos1 = find(file, 1);
        if (pos3 != -1 && pos2 != -1 && pos_slash != -1 && pos1 != -1)
            file.seekg(pos1);
        else {
            cout << "No messages";
            return;
        }
       
        string line_temp;
        string line;
        while (getline(file, line_temp)) {
            line = line + line_temp;
            line.push_back('\n');
        }

       
        if (isCorrecrt(line, pos2, pos3)) {

            file.clear();


            file.seekg(0, ios::end);

            //queue.push_back(make_pair(line, priority));
            deque<string>* deq = find_deq(priority);
            if (deq)
                deq->push_back(line);

            else {
                throw exception("Not equal to length");
            }
        }
        
        
    };

    

    void check_new(ifstream& file, int currentPos, int priority){

        
        // переместить указатель чтения в конец файла
        file.seekg(0, file.end);

        // получить текущую длину файла
        int length = file.tellg();
      
        if (length > currentPos)
        {
            setToLastLine(file, priority);
            
        }

    }
    string GetNextRecord() {
        
        //return_all_records();

        switch (state) {
        case 1:
            state++;
            return speci_msgs.back();
        case 2:
            state++;
            return metar_msgs.back();
        case 3:
            state++;
            return kn01_msgs.back();
        case 4:
            state++;
            return awos_msgs.back();
        case 5:
            if(speci_msgs.size() == 1)
                state++; 
            return speci_msgs.back();
        case 6:
            if (metar_msgs.size() == 1)
                state++;
            return metar_msgs.back();
        case 7:
            if (kn01_msgs.size() == 1)
                state++;
            return kn01_msgs.back();
        
    
    }
    }


    void start_server() {
        // Проверяем наличие новых записей в speci.txt

        ifstream speci_file(speci_file_name, ios_base::binary);
        ifstream metar_file(metar_file_name, ios_base::binary);
        ifstream kn01_file(kn01_file_name, ios_base::binary);
        ifstream awos_file(awos_file_name, ios_base::binary);
        setToLastLine(speci_file, 1);
        setToLastLine(metar_file,2);
        setToLastLine(kn01_file,3);
        setToLastLine(awos_file,4);


        // сохранить текущую позицию чтения
        int current_pos_speci = speci_file.tellg();
        int current_pos_metar = metar_file.tellg();
        int current_pos_kn01 = kn01_file.tellg();
        int current_pos_awos = awos_file.tellg();
      
        // таймер
        int k = 0;
        while (k<2)
        {
            k++;
            
            check_new(speci_file, current_pos_speci, 1);
            current_pos_speci = speci_file.tellg();
            check_new(metar_file, current_pos_metar, 2);
            current_pos_metar = metar_file.tellg();
            check_new(kn01_file, current_pos_kn01, 3);
            current_pos_kn01 = kn01_file.tellg();
            check_new(awos_file, current_pos_awos, 4);
            current_pos_awos = awos_file.tellg();
            this_thread::sleep_for(10s);


        }
   

    }

    void send_msgs(const string &msg) {
        cout << msg << endl;
    }



    void send_queu(deque<string> &msgs) {
        while (!msgs.empty()) {
            send_msgs(msgs.front());
            msgs.pop_front();
        }
    }

    void return_all_records() {
        state = 1;
        send_msgs(speci_msgs.back());
        send_msgs(metar_msgs.back());
        send_msgs(kn01_msgs.back());
        send_msgs(awos_msgs.back());
        speci_msgs.pop_back();
        metar_msgs.pop_back();
        kn01_msgs.pop_back();
        awos_msgs.pop_back();
        send_queu(speci_msgs);
        send_queu(metar_msgs);
        send_queu(kn01_msgs);


    }
};

bool connection_to_server() {
    return true;
}

void SendToServer(const string& message) {
    if (connection_to_server()) {
        // Send
    }
}


int main() {
    MeteoStation ms;
  
    ms.start_server();

    while (true) {
        auto messageToSend = ms.GetNextRecord();
        SendToServer(messageToSend);
    }


    return 0;
}
