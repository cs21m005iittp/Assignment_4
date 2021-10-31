/*


Q1.Consider matrix vector product where matrix is of large size expensive to be stored in single process so uniformly distribute matrix on multiple process
and perform marix vector product:
challenges
1.balanced distribution of matrix among process
2.gathering result 
3.same copy of vector present on each process for matrx vector product 

. Input: Size of the vector. Output: Scalar value (Print only once).
*/
#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>

double random_number_generator()
{
   double lbound = 0;
   double ubound = 10;
  
     return rand()%11;

}

int main(int argc, char** argv) {

	int n=0;
	n=atoi(argv[1]);
	
	
    // Initialize the MPI environment
    
    
    MPI_Init(&argc, &argv);
	//printf("%d,%c",argc,** argv);


    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    
    clock_t start, end;
     double cpu_time_used;
    
    start = clock();
    
   
    int elements=n/world_size;
    double vector[n];
	
	
	if(n>0)
	{
		//dividing by rows 
		int rows=n/world_size;
		int Extra_remaining_rows=n%world_size;
		if(Extra_remaining_rows>0&&rows>0)
		{
		
			if((world_size-world_rank)<=Extra_remaining_rows)//checking here remaining rows distributed in current process or not
			{
				rows+=1;
			}
		}
		
		//calculating rows taken by neibour allocatedrowsByNeibour
		int allocatedElementByNeibour=0;
	for(int i=0;i<=world_rank-1;i++)
	{
		if((world_size-i)<=Extra_remaining_rows)//checking here remaining element distributed in current process or not(for balanced distribution)
		{
			allocatedElementByNeibour+=1+rows;
		}
		else{
			allocatedElementByNeibour+=rows;
		}
	}
		
		if(rows>0)//checking whether i got some data to process
		{
			//declare matrix 
			double A[rows][n];
			double Result[n];
			int start_index=allocatedElementByNeibour;
			//int end_index=allocatedElementByNeibour+rows;//index for final result generation by specfic process
			//generate random data for matrix dim i got
			for(int i=0;i<rows;i++)
				{
					for(int j=0;j<n;j++)
					{
						A[i][j]=random_number_generator();
					}
				}
			//if im process 0 then i will create vector which used by all other process for computation
			if(world_rank==0)
			{
				//generate vector
				//send to other process
				for(int i=0;i<n;i++)
				{
					vector[i]=random_number_generator();
				}
				//broadcast vector to other process
				
				MPI_Bcast(vector,n,MPI_DOUBLE,0,MPI_Comm communicator);
				
				
			}
			{
				MPI_Barrier(MPI_COMM_WORLD);//no process start computation until vector generated by process zero  send 
				
			
			}
			//start computation:
			#pragma omp parallel
			for(int i=0;i<rows;i++)
			{
				for(int j=0;j<n;j++)
				{
					Result[i+start_index]+=A[i][j]*vector[j];
				}
				
			}
			
			//MPI_Gather(void* send_data,int send_count,MPI_Datatype send_datatype,void* recv_data,int recv_count,MPI_Datatype recv_datatype,int root,MPI_Comm communicator)
			MPI_Gather(Result,n,MPI_DOUBLE,Result,n,MPI_DOUBLE,0,MPI_COMM_WORLD);
			if(world_rank==0)
			{
				cout<<"result:"<<endl;
				for(int j=0;j<n;j++)
				{
					cout<<" "<<Result[j];
				}
			}
			
		}
	}
	else
	{
		if(world_rank==0)
		{
			cout<<"invalid input received"<<endl;
		}
	}
	
           
           
	 	   
    // Finalize the MPI environment.
    MPI_Finalize();
}