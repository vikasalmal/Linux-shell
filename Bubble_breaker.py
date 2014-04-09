import numpy
from random import randint
from sets import Set
from operator import itemgetter

	

	
''' function to rearrange the matrix after blast '''
def plast(M,block,score,pseudomode):
	size=len(block)
	elm= block.pop()
	minj=elm[1]
	maxj= elm[1]
	block.add(elm)


	for i in block:	#finding the range of columns affected
		if(i[1]<minj):	
			minj=i[1]	
		if(i[1]>maxj):
			maxj=i[1]
			
	for j in range(minj,maxj+1):	#rearranging the affected columns
		for i in range(0,size_x):
			if(M[i,j]==0):
				k=i
				while(k>=1):
					M[k,j]=M[k-1,j]
					M[k-1,j]=0
					k=k-1
		if(M[size_x-1,j]==0): #if all entries in the column black, collapse that column!
			k=j
			while(k>=1):
				for l in range(0,size_x):
					M[l,k]=M[l,k-1]
					M[l,k-1]=0
				k=k-1
	if(not pseudomode):	#if blasting is real
		#print M,size*size	#uncomment to print current matrix state and current score increment
		finallist[0].append((M.copy(),size*size))	#adding tuple (M_i,k_i) to list finallist[0]
		
	
''' function which blasts the given block ,then calls plast! '''
def blast(M,block,score,pseudomode):

	size=len(block)
	score=score+( size * size )
	for i in block:
		M[i]=0	#make each entry of the block zero since it has blasted
		
	plast(M,block,score,pseudomode) #call plast to rearrange the matrix!
	return score	#return the score!
	
''' used by get_blocks '''
def getallblocks(M,c,i,j,block):
	
	if(M[i][j]==0 or M[i,j]!=c):
		return;
	
	block.add((i,j))
	
	M[i,j]=M[i,j]+no_color
	
	if(i<size_x-1):
		getallblocks(M,c,i+1,j,block)
	if(j<size_y-1):
		getallblocks(M,c,i,j+1,block)
	if(i>=1):	
		getallblocks(M,c,i-1,j,block)
	if(j>=1):
		getallblocks(M,c,i,j-1,block)

''' function to get all the possible blocks that can blast '''	
def get_blocks(M):
	list_blocks=[[] for i in range(no_color)]
	for i in range(0,size_x):
		for j in range(0,size_y):
			if(M[i][j]!=0 and M[i][j]<=no_color):
				block=Set()
				getallblocks(M,M[i,j],i,j,block)
				
				if(len(block)>1):
					list_blocks[int(M[i,j]-no_color)-1].append(block)

	for i in range(0,size_x):
		for j in range(0,size_y):
			if(M[i][j]!=0):
				M[i,j]=M[i,j]-no_color	
	return list_blocks

''' returns the weight of the matrix by calculating the square of length of all blastable blocks '''
def weight_blocks(M):
	weight=0
	list_of_blocks=get_blocks(M)
	for i in range(0,len(list_of_blocks)):
		for j in range(0,len(list_of_blocks[i])):
			weight=weight+(len(list_of_blocks[i][j]) * len(list_of_blocks[i][j]))
	return weight
	
''' strategy implementer '''	
def strat(M,score):
	list_of_blocks=get_blocks(M)

	flag=True
	
	for i in range(0,len(list_of_blocks)): #checking if there is something to blast
		if(len(list_of_blocks[i])!=0):
			flag=False
			break
	
		
	if(flag):	#end of game if nothing more to blast
		#print score #uncomment to print final score!
		finallist[1].append(score)  #adding final score to finallist[1]
		
		return
	
	
	for i in range(0,len(list_of_blocks)):  #sorting out the blocks on basis of length for each color
		for j in range(0,len(list_of_blocks[i])):
			for k in range(0,len(list_of_blocks[i])):
				if(len(list_of_blocks[i][j])<len(list_of_blocks[i][k])):
					t=list_of_blocks[i][j]
					list_of_blocks[i][j]=list_of_blocks[i][k]
					list_of_blocks[i][k]=t

	seq=[]
	for i in range(0,len(list_of_blocks)): #getting a list of tuples '(color,total length of blocks of that color)'
		block_size=0
		for j in range(0,len(list_of_blocks[i])):
			block_size=block_size+len(list_of_blocks[i][j])
		seq.append((i,block_size))
	
	seq=sorted(seq,key=itemgetter(1)) #sorting the tuples on basis of total block size
	
	indx=0	
	while(seq[indx][1]==0):
		indx=indx+1;
	min_block_size=seq[indx][1]	#getting the minimum non-zero total block size
	pseudo_score=0	
	while(indx<len(seq) and seq[indx][1]==min_block_size):	#for all those tuples whose total block size is equal to the minimun total block size
		i=seq[indx][0]	#get the color
	
		for j in list_of_blocks[i]:	#for that color break each blocks one by one and remember the block which gives maximum weighted matrix
			
			pseudoM=numpy.zeros(shape=(size_x,size_y))
			pseudoM=M.copy()
			_score=blast(pseudoM,j,0,True) #pseudo breaking of blocks; does not affect real matrix
			_score=weight_blocks(pseudoM)
			
			if(_score>=pseudo_score):
				pseudo_score=_score
				break_block=j	
			del pseudoM
		indx=indx+1
	
	
	score=blast(M,break_block,score,False) #real breaking of the block which gets selected above
	strat(M,score)	#applying the sama strategy for resultant matrix

''' the Solve function '''
def Solve(M):
		
			

	'''---------matrix attributes---------'''
	 
	global size_x
	global size_y
	global no_color
	global list1
	global finallist
#	global score
	finallist=[[] for i in range(2)]
	
	M=numpy.array(M,int)	#converting all entries to integer
	size_x=M.shape[0]	#the row dimension
	size_y=M.shape[1]	#the column dimension
	
	
	no_color=M.max()	#getting no. of colors
	
	strat(M,0)	#calling mine strategy implementer
	return finallist[1][0]
#	return finallist #returning the final list[ [ (M_1,s_1) , (M_2,s_2) , (M_3,s_3)........(M_k,s_k) ], [final_score] ]

