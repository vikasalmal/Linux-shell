//LISP_INTERPRETER

#include<iostream>
#include<string.h>
#include<sstream>
#include<stdlib.h>
#include<math.h>
using namespace std;

class genlistnode                    //  THE MAIN NODE OF THE INTERPERTER
{
   public:
	int tag;
	string s;
	genlistnode *next,*down;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

genlistnode* makelist()
{
	genlistnode *head,*g,*cur;
	string st;
	head=NULL;
	g=NULL;
	cur=NULL;
	cin>>st;

	while(st!=")")
	{
		
		cur=new genlistnode;

		if(st=="(")
		{
			cur->tag=1;
			cur->next=NULL;
			cur->down=makelist();
		}
		
		else
		{
			cur->tag=0;
			cur->s=st;
			cur->next=NULL;
			cur->down=NULL;
		}		
		
		if(g!=NULL)	
		 	g->next=cur;
		g=cur;
		
		if(head==NULL)
			head=cur;
		cin>>st;

	}
	return head;	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


class value
{
   public:
	int tag;
	float val;
	genlistnode *link;		

};

class env
{								//
   public:
	value *table[26];
	env* parent;

	env()        //THE CONSTRUCTOR	
	{
		for(int i=0;i<26;++i)
			table[i]=NULL;
		parent=NULL;			
	}

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void addtoenv(char x, value val_new,env &e)				
{								//IT ADDS A LETTER TO THE ENVIRONMENT
	value *p = new value;
	p->tag = val_new.tag;
	p->link = val_new.link;
	p->val = val_new.val;
		
	e.table[x-'a']=p;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

value findinenv(char x, env &e)
{
	if (e.table[x-'a']!=NULL)					//IT CHECKS WHETHER A LETTER IS ALREADY PRESENT IN THE LIST
		return *(e.table[x-'a']);				//AND WHAT IS ITS VALUE

	else if(e.parent==NULL)
	{
		cout<<"ENTRY "<<x<<"NOT FOUND IN ENVIRONMENT "<<endl;
	   	exit(0);
	}
	else
		return findinenv(x,*e.parent);	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

value evallist(genlistnode *p, env &e);

value evalnode(genlistnode *p, env &e)
{
								// IT CHECKS THE CASE WHEN THE GENLISTNODE POINTER IS AN ALPHABET, DIGIT, 										ANOTHER LIST
	if(isalpha(p->s[0]))		
		return findinenv(p->s[0],e);
	
	else if(isdigit(p->s[0]))
	{
		stringstream ss(p->s);
		float l;
		ss>>l;
		value v;
		v.tag=0;
		v.val=l;
		return v;
	}	

	else if(p->tag==1)
	{
		return evallist(p->down,e);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool isOperator(char ch)
 { 
	if(ch=='+' || ch=='*'||ch=='-' ||ch=='/' ||ch=='<' ||ch=='>' ||ch=='=' ||ch=='^' ||ch=='%')
		return true;
	else
		return false;
 }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

value evallist(genlistnode *p, env &e)
{
    if(isalpha(p->s[0]))
    {
	if(p->s=="begin")
	{
		p=p->next;
		while(p->next!=NULL)
		{
			evalnode(p,e);
			p=p->next;
		}	
		return evalnode(p,e);
	}

	else if(p->s=="if")
	{
		p=p->next;
		value v= evalnode(p,e);
		if(v.val!=0)		
			return evalnode(p->next,e);
		else
			return evalnode(p->next->next,e);	       			
	}
	else if(p->s=="define")
	{
		value v=evalnode(p->next->next,e);
		addtoenv(p->next->s[0],v,e);
		return v;
	}
	else if(p->s=="set")
	{
		value v=evalnode(p->next->next,e);
		addtoenv(p->next->s[0],v,e);
		return v;	
	}
	else if(p->s=="while")
	{
		value m;
		p=p->next;
		value v=evalnode(p,e);
		while(v.val!=0)
		{
			m=evalnode(p->next,e);
			v=evalnode(p,e);
		}
		return m;
	}
	else if (p->s == "lambda") 
	{
		value v;					//THE POINTER TO THE LAMBDA FUNCTION IS STORED IN THE POINTER OF THE 
                v.tag=1;					// FUNCTION NAME...AND WHEN THE ACTUAL FUNCTION CALL IS MADE
		v.link=p;					// IT IS USED TO LOCATE THE FUNCTION BODY
		return v;
	}

        else if(p->s[0]-'a'>=0 && p->s[0]-'a'<=26)
	{
		value v;					// THE FUNCTION BODY IS EVALUATED HERE...FOR EXAMPLE...
		v= findinenv(p->s[0],e);			//  LAMBDA ( X Y ) ( + X Y )
		genlistnode * arg_list,*body;		        //  ARGUMENT LIST AND THE FUNCTION BODY IS EVALUATED HERE...
		p=p->next;					//  	
		arg_list=v.link->next->down;
		body=v.link->next->next;

		env e1;
		e1.parent =&e;		     	
		
		while(arg_list!=NULL)	
		{
					
			addtoenv(arg_list->s[0],evalnode(p,e),e1);
			p=p->next;
				arg_list=arg_list->next;
		}

		return evalnode(body,e1);	

	 }
     }	    
	
	
     else if(isOperator(p->s[0]))
     {
	value v1,v2,v;						// ALL THE OPERATORS ARE DEFINED AND CHECKED HERE IN THIS ELSE PART
	v1=evalnode(p->next,e);
	v2=evalnode(p->next->next,e);
	v.tag=0;	
	if(p->s=="+")	
		v.val=v1.val+v2.val;
	else if(p->s=="-")
		v.val=v1.val-v2.val; 
	else if(p->s=="*")
		v.val=v1.val*v2.val;
	else if(p->s=="/")
		v.val=v1.val/v2.val;
	else if(p->s=="^")
		v.val=pow(v1.val,v2.val);
	else if(p->s=="%")
		v.val=(int)v1.val%(int)v2.val;
	else if(p->s=="==")
	{
		if(v1.val==v2.val)
			v.val=1;
		else
			v.val=0;
	}
	else if(p->s=="<")
	{	
		if(v1.val<v2.val)
			v.val=1;
		else
			v.val=0;
	}	
	else if(p->s==">")
	{	
		if(v1.val>v2.val)
			v.val=1;
		else
			v.val=0;
	}	
	else if(p->s=="<=")
	{	
		if(v1.val<=v2.val)
			v.val=1;
		else
			v.val=0;
	}	
	else if(p->s==">=")
	{	
		if(v1.val>=v2.val)
			v.val=1;
		else
			v.val=0;
	}	
	return v;
     }

	else
	{
	     value v;
		v.link=p->down;					// CASE WHERE THE FUNCTION F IS NOT DEFINED AND DIRECT LAMBDA IS CALLED
		genlistnode * arg_list,*body;		        // INSTEAD THE FUNCTION IS EVALUATED DIRECTLY
		arg_list=v.link->next->down;
		body=v.link->next->next;
		p=p->next;
						
		env e1;
		e1.parent =&e;		     	
		
		while(arg_list!=NULL)	
		{
					
			addtoenv(arg_list->s[0],evalnode(p,e),e1);
			p=p->next;
				arg_list=arg_list->next;
		}

		return evalnode(body,e1);	
	}	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
	genlistnode *head;
	cout<<"ENTER THE EXPRESSION : ";
	string s;	

	cin>>s;
	if(s=="(")
		head=makelist();

	env e;
	value v=evallist(head,e);
	cout<<v.val<<"\n";		

}


