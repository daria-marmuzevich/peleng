#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <queue>
using namespace std;

class MeteoStation {

public:
    vector<pair<string, int>> queue;
    string speci_file_name = "Send/speci.txt";
    string metar_file_name = "Send/metar.txt";
    string kn01_file_name = "Send/kn01.txt";
    string awos_file_name = "Send/awos.txt";


    void isCorrecrt(string line) {

        cout << endl;
        char g[4];
        line.copy(g, 4, 6);
        uint16_t result = *reinterpret_cast<uint16_t*>(g);
        cout << result << endl;

        /*std::vector<uint16_t> bytes;
        size_t pos = 0;
        while (pos < line.length()) {
            string byte_str = line.substr(pos + 1, 2);
            int byte = std::stoi(byte_str, 0, 16);
            bytes.push_back(static_cast<uint16_t>(byte));
            pos += 3;
        }
        for (int i = 0; i < bytes.size(); i++) {
            cout << bytes[i] << " ";
        }*/
        /*int length = std::stoi(line.substr(6, 4), nullptr, 16);
        cout << length << endl;
        cout << line.substr(6, 4);*/
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
        bool found3 = true;
        bool found1 = false;
        bool found = false;
        int len = 0;
        while (!found && fileSize > 0)
        {
            len++;
            file.seekg(--fileSize);
            char c = file.get();
            if (uint16_t(c) == 3 && found3) {
                len = 1;
                found3 = false;
            }
            if (uint16_t(c) == 2) {
                fileSize -= 4;
                found1 = true;
            }
            if (uint16_t(c) == 1)
                found = true;
        }

        // если нашли начало строки, то переместить указатель чтения на эту позицию
        if (found && found1)
            file.seekg(fileSize);

       
        string line_temp;
        string line;
        while (getline(file, line_temp)) {
            line = line + line_temp;
            line.push_back('\n');
        }

        cout << line << endl;
        isCorrecrt(line);
        
        file.clear();
        //char* l = new char[len];
        //file.read(l, len);
        //cout << l[len-1];
        //cout << l << endl;
        //cout << fsize - fileSize << "****";
      
        file.seekg(0, ios::end);

        //cout << file.tellg()<<"****" << endl;
        queue.push_back(make_pair(line, priority));
        
    };

    void printQueue() {
        cout << "size - " << queue.size() << endl;
     /*   while (!queue.empty()) {
            char* s = queue.front();
            cout << s << endl;
            queue.pop();
        }*/
    }

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

    void GetNextRecord() {
        // Проверяем наличие новых записей в speci.txt

        ifstream speci_file(speci_file_name, ios_base::binary);
        ifstream metar_file(metar_file_name, ios_base::binary);
        ifstream kn01_file(kn01_file_name, ios_base::binary);
        ifstream awos_file(awos_file_name, ios_base::binary);
        //setToLastLine(speci_file, 1);
        setToLastLine(metar_file,2);
        //setToLastLine(kn01_file,3);
        //setToLastLine(awos_file,4);


        // сохранить текущую позицию чтения
        int current_pos_speci = speci_file.tellg();
        int current_pos_metar = metar_file.tellg();
        int current_pos_kn01 = kn01_file.tellg();
        int current_pos_awos = awos_file.tellg();

        //типо таймера
        int k = 0;
        while (k < 2)
        {
            k++;

            //check_new(speci_file, current_pos_speci, 1);
            //current_pos_speci = speci_file.tellg();
            check_new(metar_file, current_pos_metar, 2);
            current_pos_metar = metar_file.tellg();
          /*  check_new(kn01_file, current_pos_kn01, 3);
            current_pos_kn01 = kn01_file.tellg();
            check_new(awos_file, current_pos_awos, 4);
            current_pos_awos = awos_file.tellg();*/



        }
        cout << "Records: \n";

        return_all_records();

    }
    void send_msgs(string msg) {
        //cout << msg << endl;
    }

    void remove(std::vector<pair<string, int>>& v, size_t index) {
        v.erase(v.begin() + index);
    }

    void return_all_records() {
        for (int i = queue.size() - 1; i >= 0; i--) {
            if (queue[i].second == 1) {
                send_msgs(queue[i].first);
                remove(queue, i);
                break;
            }
        }
        for (int i = queue.size() - 1; i >= 0; i--) {
            if (queue[i].second == 2) {
                send_msgs(queue[i].first);
                remove(queue, i);
                break;
            }
        }
        for (int i = queue.size() - 1; i >= 0; i--) {
            if (queue[i].second == 3) {
                send_msgs(queue[i].first);
                remove(queue, i);
                break;
            }
        }
        for (int i = queue.size() - 1; i >= 0; i--) {
            if (queue[i].second == 4) {
                send_msgs(queue[i].first);
                remove(queue, i);
                break;
            }
        }
        for (int i = queue.size() - 1; i >= 0; i--) {
            if (queue[i].second == 1) {
                send_msgs(queue[i].first);
                remove(queue, i);
            }
        }
        for (int i = queue.size() - 1; i >= 0; i--) {
            if (queue[i].second == 2) {
                send_msgs(queue[i].first);
                remove(queue, i);
            }
        }    
        for (int i = queue.size() - 1; i >= 0; i--) {
            if (queue[i].second == 3) {
                send_msgs(queue[i].first);
                remove(queue, i);
            }
        }
    }
};


int main() {
    MeteoStation ms;
    ms.GetNextRecord();
    ms.printQueue();
    


    return 0;
}
