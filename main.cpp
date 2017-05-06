#include<vector>
#include<fstream>
#include<string>
#include<iostream>
#include<iomanip>
#include<sstream>
using namespace std;

const string password="142170142135142079";
const int ascii=256;
int tcount[ascii]; //Array to store the frequency of each character within the file
string key[ascii]; //Array to store the prefix of each character(part of huffman)

class huffman
{
	public:
		string inFile,outFile,outcode;
		char *c;
		
		huffman()
		{inFile=outFile=outcode="";
		c=new char;
		}
		void lcount();
		string makeoutput();
		void convertoutput(ofstream& outf);
		string convertinput(char c);
		~huffman()
		{delete c;
	//	c=NULL;
		}
};

struct node //Basic linked list used in huffman encoding
{
	char ch;
	int frequency;
	node* left;
	node* right;
};

node* makenode(char a,int x) //Function to make nodes....DUH!
{
	node* tmp=new node;
	tmp->ch=a;
	tmp->frequency=x;
	tmp->left=NULL;
	tmp->right=NULL;
	return tmp;
}

void nodesort(vector<node*>& b) //Function to sort nodes within a vector based on the frequency of the character.Arranged in ascending order
{
	node* tmp;
	for(int i=0;i<b.size();i++)
	{
		for(int j=0;j<b.size();j++)
		{
			if(b[i]->frequency > b[j]->frequency)
			{
				tmp=b[i];
				b[i]=b[j];
				b[j]=tmp;
			}
		}
	}
}

void huffman::lcount() //Function to count the freqeuncy of each letter within the file
{
	ifstream fin(inFile.c_str());
	ofstream fout(outFile.c_str());
	char d;
	fin>>noskipws;
	while(fin>>d)
	{
		fout<<d;
		tcount[d]++;
	}
	fin.close();
	fout.close();
}

void maketree(vector<node*>& t) //Function which accepts a vector which contains nodes and then makes a huffman tree out of them
{
	while(t.size()>1)
	{
		nodesort(t);
		node* tmp=new node;
		tmp->ch=-1;
		tmp->frequency=t[0]->frequency+t[1]->frequency;
		tmp->left=t[0];
		t.erase(t.begin());
		tmp->right=t[0];
		t.erase(t.begin());
		t.push_back(tmp);
	}
}

void makecode(node* c) //Function that accepts the node pointer which is the top of the tree and makes prefixes for each character by traversing the tree
{
	static string code="";
	
	if(c->right!=NULL)
	{
		code+="0";
		makecode(c->right);
		code=code.substr(0,code.size()-1);
	}
	if(c->left!=NULL)
	{
		code+="1";
		makecode(c->left);
		code=code.substr(0,code.size()-1);
	}
	if(!c->right && !c->left)
	{
		key[c->ch]=code;
	}
}

string huffman::makeoutput() //Function which reads the input file and produces the prefix for each character. It takes the prefixes
{							//of each character as a string and combines them to form the final code that will be outputted to the compressed file.
	string str="";
	char a;
	ifstream fin(inFile.c_str());
	fin>>noskipws;
	while(fin>>a)
	{
		str+=key[a];
	}
	fin.close();
	str+=key[3];
	return str;
}

void huffman::convertoutput(ofstream& outf) //Takes the code that is to be outputted and converts the prefix for each character
{											//which is a string, and converts it to a char
	int shift=0;
	outf<<noskipws;
	
	while(outcode.length())
	{
		if(outcode[0]=='1')
		*c|=1;
		*c<<=1;
		shift++;
		outcode.erase(0,1);
		
		if(shift==7)
		{
			shift=0;
			outf<<*c;
			delete c;
			c=new char;
		}
	}
	
	if(shift>0)
	{
		*c<<=(8-shift);
		outf<<*c;
		delete c;
		c=new char;
	}
	outf.close();
}

string huffman::convertinput(char c) //Converts the characters within a compressed file back the their string forms to produce the unformatted code
{
	stringstream ss;
	int size=sizeof(char)*8;
	
	for(int s=0;s<size-1;s++)
	{
		ss<<((c >> (size - 1 - s) ) & 1); //It is the most important part of the decompression part
	}
	return ss.str();
}

int main() //Read the main file while remebering the purpose of each function.
{
	string pass,jbits="",obits="";
	string tmp;
	int choice,ccount;
	huffman obj;
	vector<node*>v;
	char inChar;
	cout<<"What do want to do?"<<endl;
	cout<<"1) Compress"<<endl;
	cout<<"2) Decompress"<<endl;
	cout<<"3) Combine"<<endl;
	cout<<"Press either 1,2 or 3"<<endl;
	cin>>choice;
	switch(choice)
	{
	case 1:
		{
		
		cout<<"Enter name of file"<<endl;
		cin>>obj.inFile;
		obj.inFile = obj.inFile + ".txt";
		ifstream fin(obj.inFile.c_str());
		if(!fin.is_open())
		{
			cout<<"The file could not be opened"<<endl;
			return 1;
		}
		fin.close();
		tmp = obj.inFile +"compress.txt";
		ofstream fout(tmp.c_str());
	
		for(int i=0;i<ascii;i++)
		tcount[i]=0;
	
		obj.lcount();
		if(tcount[3]==0)
		tcount[3]=1;
		
		fout<<password<<endl;
		fout<<obj.inFile<<endl;
		for(int i=0;i<ascii;i++)
		{
			fout<<tcount[i]<<" ";
		}
		fout<<endl;
	
		for(int i=0;i<ascii;i++)
		{
			if(tcount[i]>0)
			{
			node* tmp=makenode(i,tcount[i]);
			v.push_back(tmp);
			}
		}
		maketree(v);
		makecode(v[0]);
	
		fout<<'#';
		obj.outcode=obj.makeoutput();
		fout<<noskipws;
		obj.convertoutput(fout);
		cout<<"Compression complete"<<endl;
		break;
		return 1;
		}
		
	case 2:
		{
			cout<<"Enter file name"<<endl;
			cin>>obj.inFile;
			obj.inFile += "compress.txt";
			ifstream fin(obj.inFile.c_str());
			if(!fin.is_open())
			{
				cout<<"The file could not be opened"<<endl;
				return 1;
			}
			fin>>pass;
			if(pass!=password)
			{
				cout<<"The password does not match. Invalid file"<<endl;
				return 1;
			}
			fin>>obj.outFile;
			obj.outFile+="decompressed.txt";
			ofstream fout(obj.outFile.c_str());
			
			for(int i=0;i<ascii;i++)
			{
				fin>>tcount[i];
				if(tcount[i]>0)
				{
					node* tmp=makenode(i,tcount[i]);
					v.push_back(tmp);
				}
			}
			
			maketree(v);
			makecode(v[0]);
			
			while(inChar!='#')
			fin>>inChar;
			
			fin>>noskipws;
			
			while(fin>>inChar)
			{
				jbits+=obj.convertinput(inChar);
			}
			
			fin.close();
			
			for(int i=0;i<jbits.length();i++)
			{
				obits+=jbits[i];
				for(int j=0;j<ascii;j++)
				{
					if(obits==key[j])
					{
						if(j==3)
						{
							fout<<"\n";
							i=jbits.length();
							break;
		                        	}
						fout<<(char)j;
						obits="";
						break;
					}
				}
			}
			
			cout<<"Decompression complete.";
			break;
		}
			
	case 3:
		{
		
		int f;
		cout<<"How many files do you want to enter ?"<<endl;
		cin>>f;
		cout<<"Enter the names of the files"<<endl;
		string *fn[f];
		for(int i=0;i<f;i++)
		{
			fn[i]=new string;
			cin>>*fn[i];
		}
		string str;
		ifstream nin;
		string filename;
		ofstream nout("Output file.txt");
		for(int i=0;i<f;i++)
		{
			filename = *fn[i] + ".txt";
			nin.open(filename.c_str());
			while(nin>>str)
			nout<<str<<" ";
			nout<<"\nEND\n";
			nin.close();
		}
		cout<<"Done";
		break;
		}
	default:
		cout << "Wrong choice" << endl;
		break;
	}
	return 1;
}
