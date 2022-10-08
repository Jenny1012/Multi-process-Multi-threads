# Multi-process-Multi-threads  
1.	開發環境 :    
C++ (在Ubuntu上執行)  
2.	實作方法和流程 :  
這次作業分成四個部分 :   
方法一 ： 將N筆資料直接Bubble Sort   
方法二 ： 將N筆資料切成K份，在一個process內對K分資料作Bubble Sort，再用同一process做Merge Sort   
方法三 ： 將N筆資料切成K份，並用K個processes分別做Bubble sort，再用process(es)做Merge Sort (k-way Merge)  
方法四 ： 將N筆資料切成K份，並用K個threads分別進行Bubble Sort後，再用thread(s)做Merge Sort  (k-way Merge)  
  
方法一的實作方法:  
		直接將檔案中的資料一次丟進Bubble Sort做排序，並輸出排好的檔案。  
  
方法二的實作方法:  
		先把指定檔案中的資料分成K份(指定份數)，再一份一份分別塞給Bubble Sort做排序，最後再將所有的資料全部收回來做Merge Sort(K-way Merge)，結束所有排序並輸出檔案。  
  
方法三的實作方法:  
		先把指定檔案中的資料分成K份(指定份數)，再一份一份分別塞給新建立的process中，在個別的process中做Bubble Sort，收回個別排好的資料，最後再將所有的資料全部收回來做Merge Sort(k-way merge)，結束所有排序並輸出檔案。  
  
方法四的實作方法:  
		先把指定檔案中的資料分成K份(指定份數)，再一份一份分別塞給新建立的thread中，在個別的thread中做Bubble Sort，收回個別排好的資料，最後再將所有的資料全部收回來做Merge Sort(k-way merge)，結束所有排序並輸出檔案。  

3.	特殊機制考量與設計  
使用者介面 : 因為方法一不需要將資料做裁切，所以使用者輸入介面的顯示順序為  
	請選擇離開或是執行(請輸入0或1) :  
	請輸入檔案名稱 :    
	請輸入方法編號(方法一、方法二、方法三、方法四) :   
	請輸入要切成幾份 :   
除了輸入順序上的改變，還加上了防呆機制，以免輸入超出原始資料量的切割份數，或是輸入非法字元。  

4.	分析結果與原因(方法一不須切割份數)    
| 分10份 | 方法一 | 方法二 | 方法三 | 方法四 |   
| 1萬筆 | 0.563047 | 0.364383 | 0.075433 | 0.289274 |   
| 10萬筆 | 57.8084 | 20.7607 | 0.560404 | 22.4596 |  
| 50萬筆 | 4022.32 | 538.361 | 4.04789 | 652.064 |  
| 100萬筆 | 11248.8 | 546.945 | 1.74323 | 592.778 |   
														(單位為秒)  
![image](https://user-images.githubusercontent.com/64779422/193533309-3a15583e-89e0-457a-bd0e-9e932f788c44.png)  
  
| 分100份 | 方法一 | 方法二 | 方法三 | 方法四 |   
| 1萬筆 | 0.563047 | 0.067268 | 0.052904 | 0.080861 |    
| 10萬筆 | 57.8084 | 2.200146 | 0.489094 | 2.96548 |  
| 50萬筆 | 4022.32 | 45.6374 | 2.52311 | 62.7267 |   
| 100萬筆 | 11248.8 | 174.623 | 5.06447 | 242.0224 |   
														(單位為秒)  
                       
![image](https://user-images.githubusercontent.com/64779422/193533378-279a1e09-7ff2-4916-94bf-5d4c19064617.png)  

  
| 分1000份 | 方法一 | 方法二 | 方法三 | 方法四 |   
| 1萬 | 0.563047 | 0.055983 | 0.268355 | 0.22097 |    
| 10萬 | 57.8084 | 0.899103 | 0.666988 | 0.880824 |   
| 50萬 | 4022.32 | 6.5807 | 2.8276 | 8.78074 |  
| 100萬 | 11248.8 | 21.0509 | 5.19767 | 30.2354 |    
														(單位為秒)  
![image](https://user-images.githubusercontent.com/64779422/193533430-badbe1d4-adcd-4136-95f7-7e277ce38721.png)  


由以上的圖表可以得知，不管分成幾份所執行的運算速度為 : 3 > 2 > 4 > 1，因為方法3使用子process做運算，而process擁有自己獨立的資料、資源與code section，在執行時不需要與其他process互相競爭存取共用資源，所以運算速度會是最快的，若是分得愈多份(愈多子process)，執行的速度愈快。反之方法4使用子thread做運算的速度會慢很多，因為每次從process中建立一個新的thread，thread與thread之間會共用同一個process中的資料、資源與code section，所以在存取共用資源時會互相影響造成delay(會有存取共用資料等待的時間)，導致運算時間會比方法三慢上許多。而方法一是直接將N份資料做Bubble Sort，Bubble Sort的時間複雜度為O(n2)，所以會是所有方法中最慢的。  
![image](https://user-images.githubusercontent.com/64779422/193533464-27cb0e08-6e6e-40d4-b214-0b4ceb3cc2ee.png)  

   
  
而根據實驗結果可以得知，分愈多份的執行時間愈快(因為單次執行的資料筆數小)，因此若想要將大筆的資料快速地做好排序，使用方法3並分割多份一點，即可達到要求。  
  
5.	撰寫程式時遇到的bug&相關的解決方法  
最後在寫方法三的時候發現，使用fork()這個system call 之後，無法得知子process執行的順序與結束的時間，因為題目要求為建立多個process執行Bubble sort，所以需要收到各個process所排完序回傳的資料，而指令fork()雖為創建一個新的子process，但是因為此指令不是共用記憶體，父子process的資源與記憶體空間皆是獨立的，所以達不到題目要求，而且fork()的process的執行順序不一定，是取決於排程演算法。因此改用指令vfork()，此指令為創建一個新的子process，且保證子process先執行後，直到exit(0)父process才能繼續執行呼叫子process後中斷的地方，因此能確保子process先做完才會輪到父process執行。  
fork():  
   
![image](https://user-images.githubusercontent.com/64779422/193533518-332d2bfd-af17-449f-b921-0a3a8c32295c.png)  
  
vfork() :  
   
![image](https://user-images.githubusercontent.com/64779422/193533568-5458a21a-2ccf-4fe7-a332-ddf0e78b1d8d.png)  

