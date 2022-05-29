/*
 *  minEditDis.cpp
 *  ���������ַ�����ı༭�����LDscore 
 *  ������
 *  1631684@tongji.edu.cn 
 */


#include"stdio.h"
#include"string.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

#define N 10
#define INF 100000000
#define min(a,b) a<b?a:b

//int dis[N][N];
//char s1[N],s2[N];
//char s11[N],s21[N];
//char s12[N],s22[N];
int dist =0;
int labellenght=0;
float LDSCORE=0; 

int minStrEditDis(char* s1, char* s2);
void csvline_populate(vector<string> &record, const string& line, char delimiter);
void  GetString( const string& Str, string &Value );


int main(int argc, char *argv[])
{
    vector<string> row1,row2;//�洢ÿһ�� �ַ��� ���ż�� �ַ����� ���� 
    string label1,label2;// 1,2,3,...,9,10,11,12,13���ֱ�ǩת����  1,2,3,...,9,A,B,C,D
    string line1,line2;// �ļ���ÿһ�� 
    string temp_str;//  row1,row2; �еĵ��� �ַ��� 
    
    // Դ�ļ�Ϊ cvs�ļ� 
    //ifstream sour("D:\\LW\\Sample_code_6_16_2012\\data_lable\\devel09_sour_lable.csv"); //�����ļ��� 
    ifstream sour("D:\\LW\\Sample_code_6_16_2012\\data_lable\\total\\data_sour_lable_total.csv"); //�����ļ��� 
    ifstream predict("D:\\LW\\Sample_code_6_16_2012\\LD score\\M_V_N.csv");   //dtw_19 �����ļ��� 
    //ifstream predict("D:\\LW\\joewan_upload_V2\\results\\my_tmpResults_Kmean3\\dtw_three_22.5_skeleon625_kmean67.csv"); 
    //ifstream predict("D:\\LW\\joewan_upload_V2\\results\\my_tmpResults_sort\\dtw_three_22.5_skeleon625.csv"); 
    //ifstream predict("D:\\LW\\joewan_upload_V2\\results\\my_tmpResults_sort\\dtw_three_22.5_skeleon625_opt.csv"); 
    //ifstream predict("D:\\LW\\joewan_upload_V2\\results\\my_tmpResults_sort\\devel09FFSK_SOMP_SVM__predict.csv"); 
    //ifstream predict("D:\\LW\\joewan_upload_V2\\results\\my_tmpResults_Kmean2\\devel20FFSK_SOMP_SVM_predict_KMEAN2.csv");
    //ifstream predict("D:\\LW\\joewan_upload_V2\\results\\my_tmpResults_Kmean2\\dtw_three_22.5_skeleon625_kmean87.csv");
    
    if (sour.fail())  { cout << "Source File not found" <<endl; return 0; }
    if (predict.fail())  { cout << "Predict File not found" <<endl; return 0; }
    long t=1;
    // �õ� �����ļ��� ÿһ�� ��ÿһ�н��бȽ�
	// devel01_11	 9 4 4 9

     while(getline(sour, line1)  && sour.good()&&getline(predict, line2)  && predict.good() )
    {
    	 // cout <<  line1 << endl; // 	devel01_1,10,, 
    	 // cout << line2 <<endl;
    	 // break;
          csvline_populate(row1, line1, ',');//  �����Ž��ַ����ֳ� vector<string>
          csvline_populate(row2, line2, ',');
       // cout << row1[1] <<endl;   // devel01_1,10,,  ------> {"devel01_1", "10"}
       // devel01_11	 9 4 4 9 ------> {"devel01_1", "10 4 4 9"}
       // if(t == 20) break;
		  
          cout<< t <<"\t";
          cout<< row1[1] << "\t";//Ϊ  Դ��ǩ 
          temp_str=row1[1];
          GetString(temp_str,label1);// 1,2,3,...,9,10,11,12,13���ֱ�ǩת����  1,2,3,...,9,A,B,C,D
          // 10 4 4 9 ------>>  A449 
          for (int it=0;it<label1.size();it++)
          cout<<label1[it]<<"|";// ��� Դ  ��ǩ��Ϣ 
          cout<<"\t";
          
          
          cout<< row2[1] << "\t";
          temp_str=row2[1];
          GetString(temp_str,label2);
          for (int it=0;it<label2.size();it++)// ʶ���ǩ 
          cout<<label2[it]<<"|";
          
          //string2char* �ַ����� 
          // string ����ת�� char* �ַ����� 
            char* c1;
            const int len1 = label1.length();
            c1 =new char[len1+1];
            strcpy(c1,label1.c_str());
            
            char* c2;
            const int len2 = label2.length();
            c2 =new char[len2+1];
            strcpy(c2,label2.c_str());
            
            int d=minStrEditDis(c1,c2); // �������� �ַ���֮��ı༭���� 
            labellenght +=label1.size();//  �� Դ��ǩ���� 
            dist+=d;// �ܱ�Ǿ��� 
            //distance+=dist;
          cout << '\t' << "edit dis: "<< d << endl;
          ++t;    
    }
    // �ر��ļ� 
    sour.close();
    predict.close();
    // ���� LDscore 
    LDSCORE=float(dist)/labellenght;
    cout<<"======LDscore is��"<<endl;
	cout<< LDSCORE <<endl;
    return 0;
    //int editDis=0;
	//	editDis=minStrEditDis(s11,s21);
	//	printf("min edit distance is: %d\n",editDis);
}

/*
���������ַ���֮��ı༭����   
��s2�ַ��� �� s1�ַ��� ��Ҫ���ٴ� ���롢ɾ�����滻���� 
*/
int minStrEditDis(char* s1, char* s2){
	int i,j;
	
	// �����ַ����ĳ��� 
	int n,m;         
	n=strlen(s1);
	m=strlen(s2);	
	int dis[n+1][m+1];// ����һ����ά���� 
	for(i=0;i<=n;i++)
		for(j=0;j<=m;j++)
			dis[i][j]=INF;//��ֵΪINF 
			
	dis[0][0]=0;// ��һ�� ��Ϊ��� ����Ϊ0 

	for(i=1;i<=n;i++)
		for(j=1;j<=m;j++)
		{
			 dis[i][j] = min(dis[i][j],dis[i-1][j]+1); //delete ɾ�� 
			 dis[i][j] = min(dis[i][j],dis[i][j-1]+1);//insert  ���� 

			//�滻 
			if(s1[i-1] != s2[j-1])// �� ��� ������ ��һƥ�䵽dis[i-1][j-1]�ľ��� + 1 
				dis[i][j] = min(dis[i][j],dis[i-1][j-1]+1);// �滻������ dis[i][j] = min(dis[i][j],dis[i-1][j-1]+2); 	
			else // ��� �Ļ������뻹��  ��һƥ�䵽dis[i-1][j-1]Ϊֹ�ľ��� 
				dis[i][j] = min(dis[i][j],dis[i-1][j-1]);
		}
    return dis[n][m];
}
   
/*
���������ַ���֮��ı༭����   
��s2�ַ��� �� s1�ַ��� ��Ҫ���ٴ� ���롢ɾ�����滻���� 
*/
int minStrEditDis1(char* sour, char* target){	
	// �����ַ����ĳ��� 
	int sl,tl;         
	sl=strlen(sour);
	tl=strlen(target);	
	int dis[sl+1][tl+1];// ����һ����ά���� 			
	dis[0][0]=0;// ��һ�� ��Ϊ��� ����Ϊ0 
	
	int i,j;
	// �߽� dp[i][0]=i,dp[0][j]=j; 
	for(i=1;i<=sl;++i)  dis[i][0]=i;
	for(j=1;j<=tl;++j)  dis[0][j]=j;

	for(i=1;i<=sl;++i)
		for(j=1;j<=tl;++j)
		{
			dis[i][j] = min(min(dis[i][j-1]+1,dis[i-1][j]+1), ((sour[i]==target[j])?(dis[i-1][j-1]):(dis[i-1][j-1]+1)));
		}
    return dis[sl][tl];
}
   
   
//  �����Ž��ַ����ֳ� vector<string>
// devel01_1,10,,  ------> {"devel01_1", "10"}
// devel01_11	 9 4 4 9 ------> {"devel01_1", "10 4 4 9"}
void csvline_populate(vector<string> &record, const string& line, char delimiter)
{
    int linepos=0;               //�ַ��������� 
    int inquotes=false;          //���� 
    char c;                      //�ַ�����ÿ��Ԫ�� char 
    int i;
    int linemax=line.length();   //ÿ���ַ������ܳ��� 
    string curstring;
    record.clear();
       
    while(line[linepos]!=0 && linepos < linemax)//�ַ�����ÿ��Ԫ�� 
    {
       
        c = line[linepos];       //�ַ����е� ÿ���ַ� 
       
        if (!inquotes && curstring.length()==0 && c=='"')//��һ������ 
        {
            //beginquotechar
            inquotes=true;//������ 
        }
        else if (inquotes && c=='"')//��������� 
        {
            //quotechar
            if ( (linepos+1 <linemax) && (line[linepos+1]=='"') )//˫���� 
            {
                //encountered 2 double quotes in a row (resolves to 1 double quote)
                curstring.push_back(c);
                linepos++;
            }
            else
            {
                //endquotechar
                inquotes=false;
            }
        }
        else if (!inquotes && c==delimiter)
        {
            //end of field
            record.push_back( curstring );
            curstring="";
        }
        else if (!inquotes && (c=='\r' || c=='\n') )
        {
            record.push_back( curstring );
            return;
        }
        else
        {
            curstring.push_back(c);
        }
        linepos++;
    }
    record.push_back( curstring );
    return;
}

//��ȡ���ַ��������ݣ����ݿո��ֲ�����������ȥ��
//Str��Դ�ַ�����Value��Ÿ���Ŀ���ַ����Ŀո���ȡ������ÿ���ַ������ݡ�
// "10 4 4 9" ---------> A449
void  GetString( const string& Str, string &Value )
{
   string Temp="";                          //�ָ���ַ��� 
   int strlength=Str.length();              //Դ�ַ������� 
   int Index= 0;
   Value="";
   char  Tmp=' '; 
   while(Str[Index]!=0 && Index < strlength)//�ַ�����ÿ��Ԫ�� 
    {
      Tmp = Str[Index];               //�ַ����е�ÿ���ַ� 
      if( Tmp==' '|| Index == strlength-1)  //�ո�Ļ����ߵ����һ���ַ� 
      {
         if( Index == strlength-1) Temp.push_back(Tmp);
         if(Temp!="")       //�����ո��ˣ���� Temp���ַ��Ļ��Ͱ�Temp���������� 
         {
         	if (Temp=="10") Temp="A";
         	if (Temp=="11") Temp="B";
         	if (Temp=="12") Temp="C";
         	if (Temp=="13") Temp="D";
         	if (Temp=="14") Temp="E";
         Value=Value+Temp;
         Temp="";         //�����һ�α������ʱ�ַ��� 
         }
        Index++; 
        continue;
      }
      else {
      Temp.push_back(Tmp);
      Index++; 
      continue;
	  }
   }
   return;
}
