# 1076038 Project 2 - Instruction scheduling
此檔案介紹如何正確的使用本程式，來達成所需要完成的事

## 執行的形式
輸入 input 檔案的檔名。

## 程式的流程
	1. 輸入指令	2. 根據cycle	3. 指定暫存器 4. 輸出結果

## FUNCTION
void algorithm(); --> 實作指令的排序

## INPUT
輸入檔包含各種指令。
* **Example:** test1.txt

		ADDI F1, F2, 1  
		SUB F1, F3, F4  
		DIV F1, F2, F3  
		MUL F2, F3, F4  
		ADD F2, F4, F2  
		ADDI F4, F1, 2  
		MUL F5, F5, F5  
		ADD F1, F4, F4  

## OUTPUT
輸出檔包含 RF、RF、RS 以及 Buffer。
* **Example:** output_10.txt  
	
		Cycle: 10  
  
		RF:   
		F1: 0 F2: 2 F3: 4 F4: 6 F5: 8   
		RAT:   
		F1: RS4 F2: RS1 F3:  F4: RS2 F5:    
		RS:  RS1: + 6 RS5   RS2: +  RS4 2   RS3    
		Buffer: X  
		 RS4: / 2 4  RS5: * 4 6   
		Buffer: (RS4) 2 / 4  

