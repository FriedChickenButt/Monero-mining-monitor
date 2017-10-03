#include <iostream>
#define _WIN32_WINNT 0x0500
#include <Windows.h>
#include <wininet.h>
#include <cstring>
#include <conio.h>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <math.h>
#include <locale>
#include <cstdlib>
#include <time.h>

using namespace std;


struct my_facet : public std::numpunct<char>
{
    explicit my_facet(size_t refs = 0) : std::numpunct<char>(refs) {}
    virtual char do_thousands_sep() const { return ','; }
    virtual std::string do_grouping() const { return "\003"; }
};


int main(int argc, char *argv[])
{
    string address;
    // Checking number of arguments
    HWND hwnd = GetConsoleWindow();
    if( hwnd != NULL )
    {
        MoveWindow(hwnd ,0,0 ,540,350 ,TRUE);
    }

    if(argc > 2)
    {
        cout << "Too many arguments supplied\n";
        system("pause");
        return 0;
    }
    else if(argc < 2)
    {
        cout << "No arguments supplied\n";
        system("pause");
        return 0;
    }
    // End of Checking number of arguments
    ifstream fin (argv[1]);

    if(!fin)
    {
        cout << "No file found\n";
        system("pause");
        return 0;
    }

    getline(fin, address);
    if(address.length() != 95)
    {
        cout << "Enter a valid address!\n";
        system("pause");
        return 0;
    }

    do
    {
        // Opening Internet connections?
        HINTERNET connect = InternetOpen("MyBrowser",INTERNET_OPEN_TYPE_PRECONFIG,NULL, NULL, 0);
        HINTERNET connect2 = InternetOpen("MyBrowser",INTERNET_OPEN_TYPE_PRECONFIG,NULL, NULL, 0);
        HINTERNET connect3 = InternetOpen("MyBrowser",INTERNET_OPEN_TYPE_PRECONFIG,NULL, NULL, 0);

        // Checking the connections (one is enough)
        if(!connect)
        {
            system("cls");
            cout << "Connection Failed: Check your Internet connection!\n";
            system("pause");
            return 0;
        }

        // API links of pool
        string apilink = "https://api.xmrpool.net/miner/"+address+"/stats/allWorkers";

        string apilink2 = "https://api.xmrpool.net/miner/"+address+"/stats";

        string apilink3 = "https://api.xmrpool.net/pool/stats/pplns";

        HINTERNET OpenAddress = InternetOpenUrl(connect, apilink.c_str(), NULL, 0, INTERNET_FLAG_PRAGMA_NOCACHE|INTERNET_FLAG_KEEP_CONNECTION, 0);

        HINTERNET OpenAddress2 = InternetOpenUrl(connect2, apilink2.c_str(), NULL, 0, INTERNET_FLAG_PRAGMA_NOCACHE|INTERNET_FLAG_KEEP_CONNECTION, 0);

        HINTERNET OpenAddress3 = InternetOpenUrl(connect3, apilink3.c_str(), NULL, 0, INTERNET_FLAG_PRAGMA_NOCACHE|INTERNET_FLAG_KEEP_CONNECTION, 0);

        if ( !OpenAddress )
        {
            //DWORD ErrorNum = GetLastError();
            system("cls");
            cout << "Failed to open URL \nTry again later\n";//Error No: " << ErrorNum << "\n";
            InternetCloseHandle(connect);
            InternetCloseHandle(connect2);
            InternetCloseHandle(connect3);
            system("pause");
            return 0;
        }

        char DataReceived[4096];
        DWORD NumberOfBytesRead = 0;

        // Storing the API data to the buffers
        stringstream buffer;
        while(InternetReadFile(OpenAddress, DataReceived, 4096, &NumberOfBytesRead) && NumberOfBytesRead )
        {
            buffer << DataReceived;
        }

        char DataReceived2[4096];
        DWORD NumberOfBytesRead2 = 0;

        stringstream buffer2;
        while(InternetReadFile(OpenAddress2, DataReceived2, 4096, &NumberOfBytesRead2) && NumberOfBytesRead2 )
        {
            buffer2 << DataReceived2;
        }

        char DataReceived3[4096];
        DWORD NumberOfBytesRead3 = 0;

        stringstream buffer3;
        while(InternetReadFile(OpenAddress3, DataReceived3, 4096, &NumberOfBytesRead3) && NumberOfBytesRead3 )
        {
            buffer3 << DataReceived3;
        }

        //cout << buffer.str();

        string data = buffer.str();
        string data2 = buffer2.str();
        string data3 = buffer3.str();

        system("cls");
        cout << "###############################################################";
        cout << "\n#----------------------MONERO POOL STATS----------------------#\n";
        cout << "###############################################################";
        cout << "\n\nCurrent Pool: xmrpool.net\n";

        // Stats before the table
        for(int i=0; data2[i] != '\0'; ++i)
        {
            // Valid shares
            stringstream ident;
            if(data2.substr(i,11) == "validShares")
            {
                cout << "\nValid Shares:\t";
                i += 13;
                for(; data2[i] != ','; i++)
                    cout << data2[i];
                //cout << "\t\t";
            }


            // Invalid shares
            if(data2.substr(i,13) == "invalidShares")
            {
                cout << "\nInvalid Shares:\t";
                i += 15;
                for(; data2[i] != ','; i++)
                    cout << data2[i];
            }

            // Amount paid
            if(data2.substr(i,7) == "amtPaid")
            {
                stringstream paid;
                double amt = 0.0;
                cout << "\nAmount paid:\t";
                i += 9;
                for(; data2[i] != ','; i++)
                    paid << data2[i];
                paid >> amt;
                amt = amt/1000000000000.0;
                cout << amt;
            }

            // Amount due
            if(data2.substr(i,6) == "amtDue")
            {
                stringstream due;
                double amt = 0.0;
                cout << "\nAmount Due:\t";
                i += 8;
                for(; data2[i] != ','; i++)
                    due << data2[i];
                due >> amt;
                amt = amt/1000000000000.0;
                cout << amt;
            }
        }

/////////////////////////////////////////////////////////////////////////////////////////////

        // Time conversion from Epoch to Human readable
        for(int i=0; data3[i] != '\0'; ++i)
        {
            // Last found
            if(data3.substr(i,18) == "lastBlockFoundTime")
            {
                stringstream time;
                cout << "\nLast Found:\t";
                i += 20;
                for(; data3[i] != ','; i++)
                    time << data3[i];
                time_t now;
                struct tm ts;
                char buf[80];

                time >> now;
                // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
                ts = *localtime(&now);
                strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
                cout << buf;
            }
        }

//////////////////////////////////////////////////////////////////////////////////////////////

        cout << "\n\nWorker\t\t\tHash/sec\t\t   Total Hashes";//t\tTotal Hashes
        cout << "\n---------------------------------------------------------------\n";

        // Data in the table
        for(int i=0; data[i] != '\0'; ++i)
        {
            // Identifier
            if(data.substr(i,9) == "identifer")
            {
                stringstream ident;
                i += 12;
                for(; data[i] != '\"'; i++)
                    ident << data[i];
                cout << setw(24) << left << ident.str();
                //cout << "\t\t";
            }


            // Hashrate
            if(data.substr(i,4) == "hash")
            {
                stringstream hash1;
                int hash2 = 0;
                i += 6;
                for(; data[i] != ','; i++)
                    hash1 << data[i];
                hash1 >> hash2;
                cout << setw(8) << right << hash2;
                cout << "\t       ";    // Seven spaces, it sucks >_<

            }

            // TotalHash
            if(data.substr(i,9) == "totalHash")
            {
                stringstream tothash;
                int hashes = 0.0;
                i += 11;
                for(; data[i] != '}'; i++)
                    tothash << data[i];
                tothash >> hashes;

                std::locale global;
                std::locale withgroupings(global, new my_facet);
                std::locale was = std::cout.imbue(withgroupings);

                cout << setw(16) << right << hashes;

                cout << "\n";
            }


        }

        // Closing all the connections/handles
        InternetCloseHandle(OpenAddress);
        InternetCloseHandle(connect);
        InternetCloseHandle(OpenAddress2);
        InternetCloseHandle(connect2);
        InternetCloseHandle(OpenAddress3);
        InternetCloseHandle(connect3);

        Sleep(10000);
    }while(1);
    //cin.get();
    return 0;
}
