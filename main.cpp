// C program to implement concurrent merge sort
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

void insertionSort(int arr[], int n);
void merge(int a[], int l1, int h1, int h2);

void mergeSort(int a[], int l, int h)
{
    int i, len=(h-l+1);

    // Using insertion sort for small sized array
    if (len<=5)
    {
        insertionSort(a+l, len);
        return;
    }

    pid_t lpid,rpid;
    lpid = fork();
    if (lpid<0)
    {
        // Lchild proc not created
        perror("Left Child Proc. not created\n");
        _exit(-1);
    }
    else if (lpid==0)
    {
        mergeSort(a,l,l+len/2-1);
        _exit(0);
    }
    else
    {
        rpid = fork();
        if (rpid<0)
        {
            // Rchild proc not created
            perror("Right Child Proc. not created\n");
            _exit(-1);
        }
        else if(rpid==0)
        {
            mergeSort(a,l+len/2,h);
            _exit(0);
        }
    }

    int status;

    // Wait for child processes to finish
    waitpid(lpid, &status, 0);
    waitpid(rpid, &status, 0);

    // Merge the sorted subarrays
    merge(a, l, l+len/2-1, h);
}

/* Function to sort an array using insertion sort*/
void insertionSort(int arr[], int n)
{
    int i, key, j;
    for (i = 1; i < n; i++)
    {
        key = arr[i];
        j = i-1;

        /* Move elements of arr[0..i-1], that are
           greater than key, to one position ahead
           of their current position */
        while (j >= 0 && arr[j] > key)
        {
            arr[j+1] = arr[j];
            j = j-1;
        }
        arr[j+1] = key;
    }
}

// Method to merge sorted subarrays
void merge(int a[], int l1, int h1, int h2)
{
    // We can directly copy  the sorted elements
    // in the final array, no need for a temporary
    // sorted array.
    int count=h2-l1+1;
    int sorted[count];
    int i=l1, k=h1+1, m=0;
    while (i<=h1 && k<=h2)
    {
        if (a[i]<a[k])
            sorted[m++]=a[i++];
        else if (a[k]<a[i])
            sorted[m++]=a[k++];
        else if (a[i]==a[k])
        {
            sorted[m++]=a[i++];
            sorted[m++]=a[k++];
        }
    }

    while (i<=h1)
        sorted[m++]=a[i++];

    while (k<=h2)
        sorted[m++]=a[k++];

    int arr_count = l1;
    for (i=0; i<count; i++,l1++)
        a[l1] = sorted[i];
}

// To check if array is actually sorted or not
void isSorted(int arr[], int len)
{
    if (len==1)
    {
        printf("Sorting Done Successfully\n");
        return;
    }

    int i;
    for (i=1; i<len; i++)
    {
        if (arr[i]<arr[i-1])
        {
            printf("Sorting Not Done\n");
            return;
        }
    }
    printf("Sorting Done Successfully\n");
    return;
}

// To fill randome values in array for testing
// purpise
void fillData(int a[], int len)
{
    // Create random arrays
    int i;
    for (i=0; i<len; i++)
        a[i] = rand()%100;
    return;
}

// Driver code
int main()
{
    int shmid;
    key_t key = IPC_PRIVATE;
    int *shm_array;


    // Using fixed size array.  We can uncomment
    // below lines to take size from user
    int length = 128;

    /* printf("Enter No of elements of Array:");
    scanf("%d",&length); */

    // Calculate segment length
    size_t SHM_SIZE = sizeof(int)*length;

    // Create the segment.
    if ((shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666)) < 0)
    {
        perror("Can't create shared memory");
        _exit(1);
    }

     //shm_array = shmat(shmid, NULL, 0);

    // Now we attach the segment to our data space.
    if ((shm_array = (int *)shmat(shmid, NULL, 0)) == (int *) -1)
    {
        perror("Can't attach shared memory");
        _exit(1);
    }

    // Create a random array of given length
    srand(time(NULL));
    fillData(shm_array, length);
    for (int i = 0; i < length; ++i) {
        printf("%d\n" , shm_array[i]);
    }

    printf("Sorted array\n");
    // Sort the created array
    mergeSort(shm_array, 0, length-1);

    for (int i = 0; i < length; ++i) {
        printf("%d\n" , shm_array[i]);
    }
    // Check if array is sorted or not
    isSorted(shm_array, length);

    /* Detach from the shared memory now that we are
       done using it. */
    if (shmdt(shm_array) == -1)
    {
        perror("shmdt");
        _exit(1);
    }

    /* Delete the shared memory segment. */
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
    {
        perror("shmctl");
        _exit(1);
    }

    return 0;
}