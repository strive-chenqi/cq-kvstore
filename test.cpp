#include<iostream>

using namespace std;

int main( void )
{
    int arry[10] = { 0, 6, 3, 2, 7, 5, 4, 9, 1, 8 };
    int i,j;
    for( i = 0; i < 10; i++)
    {
    	cout<<arry[i]<<" ";
    }	
    cout<<endl;

		
 	for (int i = 1; i < 10; i++)
	{
        int curVaule = arry[i]; //从第二个元素开始
		int preIndex = i - 1;  //第一个元素下标
        for(;preIndex>=0;preIndex--){
            if(curVaule < arry[preIndex]){
                arry[preIndex+1] = arry[preIndex];
            } else{
                break;
            }          
        }
        arry[preIndex+1] = curVaule;
    }
   
    
    for( i = 0; i < 10; i++)
    {
    cout<<arry[i]<<" ";
    }
    cout<<endl;

    return 0;
}