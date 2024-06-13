#include "copyright.h"
#include "synchconsole.h"
#include "userkernel.h"
#include "synchdisk.h"
#include "synchconsole.h"

//----------------------------------------------------------------------
// UserProgKernel::UserProgKernel
//  Interpret command line arguments in order to determine flags 
//  for the initialization (see also comments in main.cc)  
//----------------------------------------------------------------------

UserProgKernel::UserProgKernel(int argc, char **argv) 
        : ThreadedKernel(argc, argv)
{
    debugUserProg = FALSE;
    consoleIn = NULL;
    consoleOut = NULL;
    for (int i = 0; i < NumPhysPages; i++)
        PhysicalPageUsed[i] = FALSE;  // Fixed index issue
    execfileNum = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0) {
            debugUserProg = TRUE;
        }
        else if (strcmp(argv[i], "-e") == 0) {
            execfile[++execfileNum] = argv[++i];
        }
        //<TODO>
        // Get execfile & its priority & burst time from argv, then save them.
        else if (strcmp(argv[i], "-epb") == 0) {
            if (i + 3 < argc) {
                execfile[++execfileNum] = argv[++i];
                threadPriority[execfileNum] = atoi(argv[++i]);
                threadRemainingBurstTime[execfileNum] = atoi(argv[++i]);
            } else {
                cerr << "Error: Invalid arguments for -epb option" << endl;
                exit(1);
            }
        }
        //<TODO>
        else if (strcmp(argv[i], "-u") == 0) {
            cout << "===========The following argument is defined in userkernel.cc" << endl;
            cout << "Partial usage: nachos [-s]\n";
            cout << "Partial usage: nachos [-u]" << endl;
            cout << "Partial usage: nachos [-e] filename" << endl;
        }
        else if (strcmp(argv[i], "-h") == 0) {
            cout << "argument 's' is for debugging. Machine status  will be printed " << endl;
            cout << "argument 'e' is for executing file." << endl;
            cout << "argument 'u' will print all argument usage." << endl;
            cout << "For example:" << endl;
            cout << " ./nachos -s : Print machine status during the machine is on." << endl;
            cout << " ./nachos -e file1 -e file2 : executing file1 and file2."  << endl;
        }
    }
}

//----------------------------------------------------------------------
// UserProgKernel::Initialize
//  Initialize Nachos global data structures.
//----------------------------------------------------------------------

void
UserProgKernel::Initialize()
{
    ThreadedKernel::Initialize();    // init multithreading

    machine = new Machine(debugUserProg);
    fileSystem = new FileSystem();

    currentThread = new Thread("main", threadNum++);    
    synchConsoleIn = new SynchConsoleInput(consoleIn);
    synchConsoleOut = new SynchConsoleOutput(consoleOut);  

    currentThread->setStatus(RUNNING);

    interrupt->Enable();
    
    #ifdef FILESYS
        synchDisk = new SynchDisk("New SynchDisk");
    #endif // FILESYS
}

//----------------------------------------------------------------------
// UserProgKernel::~UserProgKernel
//  Nachos is halting.  De-allocate global data structures.
//  Automatically calls destructor on base class.
//----------------------------------------------------------------------

UserProgKernel::~UserProgKernel()
{
    delete fileSystem;
    delete machine;
#ifdef FILESYS
    delete synchDisk;
#endif
    delete synchConsoleIn;
    delete synchConsoleOut;
}

//----------------------------------------------------------------------
// UserProgKernel::Run
//  Run the Nachos kernel.  For now, just run the "halt" program. 
//----------------------------------------------------------------------
void
UserProgKernel::Run()
{
    InitializeAllThreads();  // Start all threads

    // ThreadedKernel::Run();
}

//----------------------------------------------------------------------
// UserProgKernel::SelfTest
//      Test whether this module is working.
//----------------------------------------------------------------------

void
UserProgKernel::SelfTest() {
/*    char ch;

    ThreadedKernel::SelfTest();

    // test out the console device

    cout << "Testing the console device.\n" 
    << "Typed characters will be echoed, until q is typed.\n"
        << "Note newlines are needed to flush input through UNIX.\n";
    cout.flush();

    SynchConsoleInput *input = new SynchConsoleInput(NULL);
    SynchConsoleOutput *output = new SynchConsoleOutput(NULL);

    do {
        ch = input->GetChar();
        output->PutChar(ch);   // echo it!
    } while (ch != 'q');

    cout << "\n";

    // self test for running user programs is to run the halt program above
*/

//  cout << "This is self test message from UserProgKernel\n" ;
}


void
ForkExecute(Thread *t)
{
    // cout << "Thread: " << (void *) t << endl;
    //<TODO>
    // When Thread t goes to Running state in the first time, its file should be loaded & executed.
    // Hint: This function would not be called until Thread t is on running state.
    t->space->Execute(t->getName());
    //<TODO>
}

int 
UserProgKernel::InitializeOneThread(char* name, int priority, int burst_time)
{
    //<TODO>
    // When each execfile comes to Exec function, Kernel helps to create a thread for it.
    // While creating a new thread, thread should be initialized, and then forked.
    Thread *newThread = new Thread(name);
    newThread->setPriority(priority);
    newThread->setRemainingBurstTime(burst_time);
    newThread->space = new AddrSpace();
    newThread->Fork((VoidFunctionPtr) &ForkExecute, (void *)newThread);
    t[threadNum] = newThread;
    //<TODO>

    threadNum++;
    return threadNum - 1;
}

void 
UserProgKernel::InitializeAllThreads()
{
    for (int i = 1; i <= execfileNum; i++){
        // cout << "execfile[" << i << "]: " << execfile[i] << " start " << endl;
        int a = InitializeOneThread(execfile[i], threadPriority[i], threadRemainingBurstTime[i]);
        // cout << "execfile[" << i << "]: " << execfile[i] << " end "<< endl;
    }
    // After InitializeAllThreads(), let main thread be terminated that we can start to run our thread.
    currentThread->Finish();
    // kernel->machine->Run();
}