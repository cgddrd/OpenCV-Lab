#include <iostream>
#include <time.h>
#include <math.h>

using namespace std;

int main()
{
    
    int num;
    int largest = 0;
    int count;
    
    cout<<"Please enter a number to have its Largest Prime Factor found"<<endl;
    cin>>num;
    //num = 600851475143;
    
    for (int factor = 1; factor < num; factor++)
    {
        if (num % factor == 0)
        {
            
            largest = factor;
            
//            count = 0;
//            for (int primetest=2; count == 0 && factor > primetest ; primetest++)
//            {
//                if (factor % primetest == 0)
//                    count ++;
//                //endif
//            }
//            if (count == 0)
//                largest = factor;
//            //endif
        }
        
    }//endif
    
    cout<<largest<<endl;
    
    system("PAUSE");
}