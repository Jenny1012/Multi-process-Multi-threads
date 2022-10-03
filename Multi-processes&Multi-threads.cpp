/* OS Project01 */

/* 方法一 --> N直接Bubble Sort 
   方法二 --> N切成K，在一個process內對K分資料作Bubble Sort，再用同一process做Merge Sort 
   方法三 --> N切成K，K個processes分別做Bubble sort，再用process(es)做Merge Sort 
   方法四 --> N切成K，並由K個threads分別進行Bubble Sort後，再用thread(s)做Merge Sort 
*/

#include <iostream>
#include <thread>
#include <ctime>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <cstring>
#include <vector>
#include <cstdlib>
#include <unistd.h>

using namespace std ;

double Start, End;
time_t now = time(0); // t passed as argument in function time()

void OutputFile( vector<int> list, string middlename, int method, float time ) {
        tm *ltm = localtime(&now);
        //cout << ltm->tm_year+1900 << " " << ltm->tm_mon+1 << " " << ltm->tm_mday << endl; 
    
	fstream outfile;
	string file = middlename + "_output" + to_string(method) + ".txt";
	char* filename = (char*)file.c_str();
	outfile.open( filename, ios::out );
	outfile << "Sort : " << endl;
	for ( int i = 0 ; i < list.size() ; i++ ) {
		outfile << list[i] << endl;
	} // for
	
        outfile << "CPU Time : " << time << endl;
        outfile << "datetime : " << ltm->tm_year+1900 << "-" << ltm->tm_mon+1 << "-" << ltm->tm_mday << " " ;
        outfile << ltm->tm_hour << ":" << ltm->tm_min << ":" << ltm->tm_sec << "+08:00" << endl;
	outfile.close();
} // OutputFile()

void BubbleSort( vector<int> &list ) {
	for ( int i = 0 ; i < list.size() - 1 ; i++ ) {
		for ( int j = 0 ; j < list.size() - 1 - i ; j++ ) {
			if ( list[j] > list[j+1] ) {
				int temp = list[j];
				list[j] = list[j+1];
				list[j+1] = temp;
			} // if
		} // for
	} // for
} // BubbleSort()

void Merge( vector<int> &list, int p, int q, int m ) {
	int n1 = m - p + 1;
	int n2 = q - m;
	vector<int> L;
	vector<int> R;
	
	for ( int i = 0 ; i < n1 ; i++ ) {
		int temp = list[p+i];
		L.push_back( temp );
	} // for
	
	for ( int i = 0 ; i < n2 ; i++ ) {
		int temp = list[i+m+1];
		R.push_back( temp );
	} // for
	
	L.push_back( 99999 ); // n1 + 1
	R.push_back( 99999 ); // n2 + 1
	
	int i = 0;
	int j = 0;
	for ( int k = p ; k <= q ; k++ ) {
		if ( L[i] <= R[j] ) {
			list[k] = L[i];
			i++;
		} // if
		else {
			list[k] = R[j];
			j++;
		} // else
	} // for
	
	L.clear();
	R.clear();
} // Merge()

void MergeSort( vector<int> &list, int p, int q ) {
	// p -> start index, r -> end index
	if ( p < q ) {
		int m = ( p + q ) / 2;
		MergeSort( list, p, m );
		MergeSort( list, m+1, q );
		Merge( list, p, q, m );
	} // if

} // MergeSort()

void Interface( vector< vector<int> > dlist, vector<int> &list, int copies, int method, string middlename ) {
	if ( method == 1 ) {
		// 方法一 --> N直接Bubble Sort
		BubbleSort( list );
	} // if
	else if ( method == 2 ) {
		// 方法二 --> N切成K，在一個process內對K分資料作Bubble Sort，再用同一process做Merge Sort 
		list.clear();
		for ( int i = 0 ; i < dlist.size() ; i++ ) {
			BubbleSort( dlist[i] );
			for ( int j = 0 ; j < dlist[i].size() ; j++ ) {
				int temp = dlist[i][j];
				list.push_back( temp );
			} // for
		} // for
		
		MergeSort( list, 0, list.size() - 1 );
	} // else if
	else if ( method == 3 ) {
		// 方法三 --> N切成K，K個processes分別做Bubble sort，再用process(es)做Merge Sort
		for ( int i = 0 ; i < copies ; i++ ) {
                    if ( vfork() == 0 ) {
			//cout << "I am a new process -> " << i << endl;
                        BubbleSort( dlist[i] );
                        //sleep(copies);
                        exit(0);
                    } // if
                } // for
                
                list.clear();
		for ( int i = 0 ; i < dlist.size() ; i++ ) {
			for ( int j = 0 ; j < dlist[i].size() ; j++ ) {
				int temp = dlist[i][j];
				list.push_back( temp );
			} // for
		} // for
		//cout << "Subprocess finish executing !!!" << endl;
		MergeSort( list, 0, list.size() - 1 );
	} // else if
	else if ( method == 4 ) {
		// 方法四 --> N切成K，並由K個threads分別進行Bubble Sort後，再用thread(s)做Merge Sort
		vector<std::thread> threads;
		for ( int i = 0 ; i < copies ; i++ ) {
			threads.push_back( std::thread( BubbleSort, std::ref( dlist[i] ) ) );
			//threads[i].join();
		} // for
		
		for ( int i = 0 ; i < copies ; i++ ) {
			threads[i].join();
		} // for
		
		list.clear();
		for ( int i = 0 ; i < dlist.size() ; i++ ) {
			for ( int j = 0 ; j < dlist[i].size() ; j++ ) {
				int temp = dlist[i][j];
				list.push_back( temp );
			} // for
		} // for
		
		MergeSort( list, 0, list.size() - 1 );
	} // else if
} // Interface()

void ReadFile( vector< vector<int> > &dlist, vector<int> &list, char* file, int method, int copies ) {
	string line;
	int num;
	vector<int> temp;
	ifstream infile ; // 讀檔案
	infile.open( file ) ;
	
	while ( getline( infile, line ) ) {
                if ( line != "" ) {
                    num = atoi( line.c_str() );
                    list.push_back( num );
                } // if
	} // while

	if ( method != 1 ) {
		int before = 0; // 之前讀進來的 
		int serving = list.size() / copies;  // 每份的量 
		for ( int i = 0 ; i < copies - 1 ; i++ ) {
			for ( int j = 0 ; j < serving ; j++ ) {
				int index = list[before + j];
				temp.push_back( index );
			} // for
			
			if ( i <= copies - 2 ) {
				dlist.push_back( temp );
				temp.clear();
			} // if
			
			before = before + serving;
		} // for
		
		serving = list.size() - before;
		for ( int i = 0 ; i < serving ; i++ ) {
			int index = list[before + i];
			temp.push_back( index );
		} // for
		
		dlist.push_back( temp );
		temp.clear();
	} // else
	
	infile.close();
} // ReadFile()

bool IllegalCheck( string middlename, int &copies ) {
	bool illegal = true;
	int range = 0;
	if ( middlename == "input_1w" ) {
		range = 10000;
	} // if
	else if ( middlename == "input_10w" ) {
		range = 100000;
	}  // else if
	else if ( middlename == "input_50w" ) {
		range = 500000;
	} // else if
	else if ( middlename == "input_100w" ) {
		range = 1000000;
	} // else if

        while ( illegal ) {
		if ( copies > range || cin.fail() ) {
			cout << "非法輸入，請重新輸入要切成幾分 : " ;
			cin.clear();
			while ( cin.get() != '\n' ) {
				continue;
			} // while
	
			cin >> copies;
		} // if
		else {
			illegal = false;
		} // else
	} // while
} // IllegalCheck()

int main() {
	vector<int> list;
	vector< vector<int> > dlist;
	int num = 0;
	int copies = 0;
	int method = 0;
	string middlename;
	string input;
	
	cout << "請選擇離開或執行(請輸入0或1) -> ";
        cin >> num;
    
	while ( cin.fail() ) {
		cout << "非法輸入, 請重新選擇離開或執行(請輸入0或1) -> ";
		cin.clear();
		while ( cin.get() != '\n' ) {
			continue;
		} // while
		cin >> num ;
	} // while

	while ( num != 0 ) {
    	if ( num == 1 ) {
    		cout << "請輸入檔案名稱 : " ;
    		string name;
    		cin >> middlename;
		input = middlename + ".txt";
		char* filename = (char*)input.c_str();
       		
       		ifstream infile ; // 讀檔案
		infile.open( filename ) ;
		while ( !infile ) { // 無此檔案
			if ( strcmp ( name.c_str(), "0") == 0  )
				break  ;
			else {
				cout << "查無此檔案!!!!請重新輸入!!!!"<< endl ;
				cout << "請輸入檔案名稱 : " ;
				cin >> middlename;
				input = middlename + ".txt";
				filename = (char*)input.c_str();
				infile.open( filename );
			} // else
   		} // while
       		
       		cout << "請輸入方法編號(方法一, 方法二, 方法三, 方法四) : ";
		cin >> method;
		while ( cin.fail() ) {
                        cout << "非法輸入, 請重新輸入方法編號(方法一, 方法二, 方法三, 方法四) : ";
                        cin.clear();
                        while ( cin.get() != '\n' ) {
                               continue;
                        } // while
                                
			cin >> method;
		} // while
			
		if ( method == 2 || method == 3 || method == 4 ) {
			cout << "請輸入要切成幾分 : ";
			cin >> copies; 
			while ( cin.fail() ) {
				cout << "非法輸入, 請重新輸入要切成幾分 : " ;
				cin.clear();
				while ( cin.get() != '\n' ) {
					continue;
				} // while

				cin >> copies;
			} // while
		} // if

		IllegalCheck( middlename, copies );
		ReadFile( dlist, list, filename, method, copies );
                Start = clock();
		Interface( dlist, list, copies, method, middlename );
                End = clock();
                float time = float(End - Start)/CLOCKS_PER_SEC ;
                OutputFile( list, middlename, method, time );
		cout << "Time --> " << time << "s" << endl;
       		list.clear();
       		dlist.clear();
		} // if
		
		cout << "請選擇離開或繼續執行(請輸入0或1) -> ";
		cin >> num;
		while ( cin.fail() ) {
			cout << "非法輸入, 請重新選擇離開或執行(請輸入0或1) -> ";
			cin.clear();
			while ( cin.get() != '\n' ) {
				continue;
			} // while
			cin >> num ;
	} // while
	} // while
	
	cout << "執行結束 !!! " << endl;
} // main()
