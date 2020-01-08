# 1076038 Project 2 - Instruction scheduling
此檔案介紹如何正確的使用本程式，來達成所需要完成的事

## 執行的形式
輸入 input 檔案的檔名。

## 程式的流程
	1. 讀入 T/NT  序列	2. 輸入 Predictor 初始狀態00	3. T/N預測 4. 計算 Misprediction rate
	5. 輸出的最終結果

## FUNCTION
bool two_bit_predictor(int index);
此 function 用來預測 T 或 NT，
並回傳至下一個狀態。

## INPUT
輸入檔包含 T(Taken) 及 NT(Not Taken)。
* **Example:** test1.txt

		T NT T T T NT NT NT NT
		NT NT T T T T NT T NT

簡單來說以上總共有兩筆的測資

## OUTPUT
輸出檔包含 Prediction、Miss or not 以及 Misprediction rate。
* **Example:** output.txt
	
State: 00 01 00 01 10 11 10 01 00 
Input: T NT T T T NT NT NT NT 
Prediction: N N N N T T T N N 
Miss or not: X O X X O X X O O 
Misprediction rate: 5/9
  
 State: 00 00 00 01 10 11 11 10 11 
 Input: NT NT T T T T NT T NT 
 Prediction: N N N N T T T T T 
 Miss or not: O O X X O O X O X 
 Misprediction rate: 4/9

