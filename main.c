#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef struct node{
    char pid[20];
    int arrival_time;
    int cpu_burst;
    int priority;
    struct node* next;
}node;

typedef struct gantt{
    char pid[20];
    struct gantt* next;
} gantt;

node* create_node(){
    node* t = (node*)malloc(sizeof(node));
    t->next = NULL;
    return t;
}

node* copy_node(node* a){
    node* b = create_node();
    b->arrival_time = a->arrival_time;
    b->cpu_burst = a->cpu_burst;
    strcpy(b->pid,a->pid);
    b->priority = a->priority;
    b->next = NULL;
    return b;
}

gantt* create_gantt(){
    gantt* t = (gantt*)malloc(sizeof(gantt));
    char s[] = ".";
    strcpy(t->pid,s);
    t->next = NULL;
    return t;
}

node *arrival = NULL, *queue = NULL;
gantt* result;

void read_file(){
    FILE* fp = fopen("data.csv","r");
    char line[100];
    while(fgets(line,100,fp)){
        node* t = create_node();
        char * token = strtok(line,",");
        strcpy(t->pid,token);
        token = strtok(NULL,",");
        t->arrival_time = atoi(token);
        token = strtok(NULL,",");
        t->cpu_burst = atoi(token);
        token = strtok(NULL,",");
        t->priority = atoi(token);
        t->next = NULL;
        if(arrival==NULL){
            arrival = t;
        }
        else{
            node * p = arrival;
            if(p->arrival_time>t->arrival_time){
                t->next = p;
                arrival = t;
            }
            else{
                while(p->next!=NULL && p->next->arrival_time<=t->arrival_time){
                    p = p->next;
                }
                t->next = p->next;
                p->next = t;
            }
        }
    }
}

void reset(){
    while(1){
        node* t = arrival;
        if(arrival==NULL) break;
        arrival = arrival->next;
        free(t);
    }
    while(1){
        node* t = queue;
        if(queue==NULL) break;
        queue = queue->next;
        free(t);
    }
    while(1){
        gantt* t = result;
        if(result==NULL) break;
        result = result->next;
        free(t);
    }
    arrival = NULL;
    queue = NULL;
    result = NULL;
}

void print_chart(gantt* head, float avgturn, float avgwait){
    FILE* fp = fopen("result.txt","w");
    gantt* t = head;
    fprintf(fp,"\nGANTT CHART:-\n");
    while(t!=NULL){
        fprintf(fp,"%2s ",t->pid);
        t = t->next;
    }
    fprintf(fp,"\n");
    fprintf(fp,"Average Turnaround time: %0.2f\n",avgturn);
    fprintf(fp,"Average Waiting time: %0.2f\n",avgwait);
    fclose(fp);
    printf("Result Generated in Result.txt\n");
}

void fcfs(){
    read_file();
    int sum_turn = 0, pcnt = 0, sum_wait = 0;
    result = NULL;
    gantt* end = NULL;
    node* p = arrival;
    node* t = NULL;
    queue = NULL;
    node* rear = NULL;
    int time = 0;
    while(p!=NULL || t!=NULL){
        while(p!=NULL && time==p->arrival_time){
            node* x = copy_node(p);
            if(queue==NULL){
                queue = x;
                rear = x;
            }
            else{
                rear->next=x;
                rear = x;
            }
            p = p->next;
        }
        t = queue;
        if(t!=NULL){
            if(t->cpu_burst==0){
                node* temp = t;
                t = t->next;
                queue = t;
                free(temp);
                continue;
            }
            if(result==NULL){
                result = create_gantt();
                strcpy(result->pid,t->pid);
                end = result;
            }
            else{
                gantt* temp = create_gantt();
                strcpy(temp->pid,t->pid);
                end->next = temp;
                end = temp;
            }
            (t->cpu_burst)--;
            if(t->cpu_burst==0){
                int turnaround_time = time - t->arrival_time + 1;
                node* temp = arrival;
                while(strcmp(temp->pid,t->pid)){
                    temp = temp->next;
                }
                int waiting_time = turnaround_time - temp->cpu_burst;
                sum_turn += turnaround_time;
                sum_wait += waiting_time;
                pcnt++;
            }
        }
        else{
            if(result==NULL){
                result = create_gantt();
                end = result;
            }
            else{
                gantt* temp = create_gantt();
                end->next = temp;
                end = temp;
            }
        }
        time++;
    }
    print_chart(result,(float)sum_turn/pcnt,(float)sum_wait/pcnt);
    reset();
}

void sjf(){
    read_file();
    int sum_turn = 0, sum_wait = 0, pcnt = 0;
    queue = NULL;
    result = NULL;
    gantt* end = NULL;
    node* t = NULL;
    node* p = arrival;
    node* rear = NULL;
    int time = 0;
    while(p!=NULL || t!=NULL){
        while(p!=NULL && time==p->arrival_time){
            node* x = copy_node(p);
            if(queue==NULL){
                queue = x;
                rear = x;
            }
            else{
                if(t==NULL){
                    if(queue->cpu_burst>x->cpu_burst){
                        x->next = queue;
                        queue = x;
                    }
                    else{
                        node* temp = queue;
                        while(temp->next!=NULL && temp->next->cpu_burst<=x->cpu_burst){
                            temp = temp->next;
                        }
                        x->next = temp->next;
                        temp->next = x;
                    }
                }
                else{
                    node* temp = t;
                    while(temp->next!=NULL && temp->next->cpu_burst<=x->cpu_burst){
                        temp = temp->next;
                    }
                    x->next = temp->next;
                    temp->next = x;
                }
            }
            p = p->next;
        }
        if(t==NULL) t = queue;
        if(t!=NULL){
            if(t->cpu_burst==0){
                node* temp = t;
                t = t->next;
                free(temp);
                queue = t;
                continue;
            }
            if(result==NULL){
                result = create_gantt();
                strcpy(result->pid,t->pid);
                end = result;
            }
            else{
                gantt* temp = create_gantt();
                strcpy(temp->pid,t->pid);
                end->next = temp;
                end = temp;
            }
            (t->cpu_burst)--;
            if(t->cpu_burst==0){
                int turnaround_time = time - t->arrival_time + 1;
                node* temp = arrival;
                while(strcmp(temp->pid,t->pid)){
                    temp = temp->next;
                }
                int waiting_time = turnaround_time - temp->cpu_burst;
                sum_turn += turnaround_time;
                sum_wait += waiting_time;
                pcnt++;
            }
        }
        else{
            if(result==NULL){
                result = create_gantt();
                end = result;
            }
            else{
                gantt* temp = create_gantt();
                end->next = temp;
                end = temp;
            }
        }
        time++;
    }
    print_chart(result,(float)sum_turn/pcnt,(float)sum_wait/pcnt);
    reset(); 
}

void rr(){
    int sum_turn = 0, pcnt = 0, sum_wait = 0;
    int q;
    printf("Enter the time quanta: ");
    scanf("%d", &q);
    read_file();
    queue = NULL;
    result = NULL;
    gantt* end = NULL;
    node* t = NULL;
    node* p = arrival;
    node* rear = NULL;
    int time = 0;
    int currq = 0;
    while(p!=NULL || queue!=NULL){
        while(p!=NULL && time==p->arrival_time){
            node* x = copy_node(p);
            if(queue==NULL){
                queue = x;
                rear = x;
            }
            else{
                rear->next = x;
                rear = x;
            }
            p = p->next;
        }
        if(t==NULL) t = queue;
        if(t!=NULL){
            if(t->cpu_burst==0){
                node* temp = t;
                t = t->next;
                queue = t;
                free(temp);
                currq = 0;
                continue;
            }
            if(result==NULL){
                result = create_gantt();
                strcpy(result->pid,t->pid);
                end = result;
            }
            else{
                gantt* temp = create_gantt();
                strcpy(temp->pid,t->pid);
                end->next = temp;
                end = temp;
            }
            (t->cpu_burst)--;
            if(t->cpu_burst==0){
                int turnaround_time = time - t->arrival_time + 1;
                node* temp = arrival;
                while(strcmp(temp->pid,t->pid)){
                    temp = temp->next;
                }
                int waiting_time = turnaround_time - temp->cpu_burst;
                sum_turn += turnaround_time;
                sum_wait += waiting_time;
                pcnt++;
            }
            currq++;
            if(currq==q){
                currq = 0;
                if(queue!=rear){
                    queue = t->next;
                    t->next = NULL;
                    rear->next = t;
                    t = queue;
                    rear = rear->next;
                }
            }
        }
        else{
            if(result==NULL){
                result = create_gantt();
                end = result;
            }
            else{
                gantt* temp = create_gantt();
                end->next = temp;
                end = temp;
            }
        }
        time++;
    }
    print_chart(result,(float)sum_turn/pcnt,(float)sum_wait/pcnt);
    reset();
}

void ps(){
    read_file();
    int sum_turn = 0, pcnt = 0, sum_wait = 0;
    queue = NULL;
    result = NULL;
    gantt* end = NULL;
    node* t = NULL;
    node* p = arrival;
    node* rear = NULL;
    int time = 0;
    int preempt;
    printf("Do you want to the scheduling to be Preemptive (Input 1 for yes anything else for no): ");
    scanf("%d", &preempt);
    while(p!=NULL || queue!=NULL){
        while(p!=NULL && time==p->arrival_time){
            node* x = copy_node(p);
            if(queue==NULL){
                queue = x;
                rear = x;
            }
            else{
                if(t==NULL){
                    if(queue->priority>x->priority){
                        x->next = queue;
                        queue = x;
                    }
                    else{
                        node* temp = queue;
                        while(temp->next!=NULL && temp->next->priority<=x->priority){
                            temp = temp->next;
                        }
                        x->next = temp->next;
                        temp->next = x;
                    }
                }
                else{
                    if(preempt==1 && t->priority>x->priority){
                        x->next = t;
                        queue = x;
                        t = x;
                    }
                    else{
                        node* temp = t;
                        while(temp->next!=NULL && temp->next->priority<=x->priority){
                            temp = temp->next;
                        }
                        x->next = temp->next;
                        temp->next = x;
                    }
                }
            }
            p = p->next;
        }
        if(t==NULL) t = queue;
        if(t!=NULL){
            if(t->cpu_burst==0){
                node* temp = t;
                t = t->next;
                queue = t;
                free(temp);
                continue;
            }
            if(result==NULL){
                result = create_gantt();
                strcpy(result->pid,t->pid);
                end = result;
            }
            else{
                gantt* temp = create_gantt();
                strcpy(temp->pid,t->pid);
                end->next = temp;
                end = temp;
            }
            (t->cpu_burst)--;
            if(t->cpu_burst==0){
                int turnaround_time = time - t->arrival_time + 1;
                node* temp = arrival;
                while(strcmp(temp->pid,t->pid)){
                    temp = temp->next;
                }
                int waiting_time = turnaround_time - temp->cpu_burst;
                sum_turn += turnaround_time;
                sum_wait += waiting_time;
                pcnt++;
            }
        }
        else{
            if(result==NULL){
                result = create_gantt();
                end = result;
            }
            else{
                gantt* temp = create_gantt();
                end->next = temp;
                end = temp;
            }
        }
        time++;
    }
    print_chart(result,(float)sum_turn/pcnt,(float)sum_wait/pcnt);
    reset();
}

void srtf(){
    read_file();
    int sum_turn = 0, pcnt = 0, sum_wait = 0;
    queue = NULL;
    result = NULL;
    gantt* end = NULL;
    node* t = NULL;
    node* p = arrival;
    node* rear = NULL;
    int time = 0;
    while(p!=NULL || t!=NULL){
        while(p!=NULL && time==p->arrival_time){
            node* x = copy_node(p);
            if(queue==NULL){
                queue = x;
                rear = x;
            }
            else{
                if(queue->cpu_burst>x->cpu_burst){
                    x->next = queue;
                    queue = x;
                }
                else{
                    node* temp = queue;
                    while(temp->next!=NULL && temp->next->cpu_burst<=x->cpu_burst){
                        temp = temp->next;
                    }
                    x->next = temp->next;
                    temp->next = x;
                }
            }
            p = p->next;
        }
        t = queue;
        if(t!=NULL){
            if(t->cpu_burst==0){
                node* temp = t;
                t = t->next;
                free(temp);
                queue = t;
                continue;
            }
            if(result==NULL){
                result = create_gantt();
                strcpy(result->pid,t->pid);
                end = result;
            }
            else{
                gantt* temp = create_gantt();
                strcpy(temp->pid,t->pid);
                end->next = temp;
                end = temp;
            }
            (t->cpu_burst)--;
            if(t->cpu_burst==0){
                int turnaround_time = time - t->arrival_time + 1;
                node* temp = arrival;
                while(strcmp(temp->pid,t->pid)){
                    temp = temp->next;
                }
                int waiting_time = turnaround_time - temp->cpu_burst;
                sum_turn += turnaround_time;
                sum_wait += waiting_time;
                pcnt++;
            }
        }
        else{
            if(result==NULL){
                result = create_gantt();
                end = result;
            }
            else{
                gantt* temp = create_gantt();
                end->next = temp;
                end = temp;
            }
        }
        time++;
    }
    print_chart(result,(float)sum_turn/pcnt,(float)sum_wait/pcnt); 
    reset();
}

void lrtf(){
    read_file();
    int sum_turn = 0, pcnt = 0, sum_wait = 0;
    queue = NULL;
    result = NULL;
    gantt* end = NULL;
    node* t = NULL;
    node* p = arrival;
    int time = 0;
    while(p!=NULL || t!=NULL){
        while(p!=NULL && time==p->arrival_time){
            node* x = copy_node(p);
            if(queue==NULL){
                queue = x;
            }
            else{
                    node* temp = queue;
                    while(temp->next!=NULL && temp->next->cpu_burst>=x->cpu_burst){
                        temp = temp->next;
                    }
                    x->next = temp->next;
                    temp->next = x;
            }
            p = p->next;
        }
        t = queue;

        if(t!=NULL){
            if(t->cpu_burst==0){
                node* temp = t;
                t = t->next;
                free(temp);
                queue = t;
                continue;
            }
            if(result==NULL){
                result = create_gantt();
                strcpy(result->pid,t->pid);
                end = result;
            }
            else{
                gantt* temp = create_gantt();
                strcpy(temp->pid,t->pid);
                end->next = temp;
                end = temp;
            }
            (t->cpu_burst)--;
            if(t->cpu_burst==0){
                int turnaround_time = time - t->arrival_time + 1;
                node* temp = arrival;
                while(strcmp(temp->pid,t->pid)){
                    temp = temp->next;
                }
                int waiting_time = turnaround_time - temp->cpu_burst;
                sum_turn += turnaround_time;
                sum_wait += waiting_time;
                pcnt++;
            }
            printf("Time: %d, Do you want to preempt?(Enter 1 for YES, any other input for NO): ",time+1);
            int pc;
            scanf("%d",&pc);
            if(pc==1){
                node* q = t;
                while(q->next!=NULL && (t->cpu_burst<q->next->cpu_burst || (t->cpu_burst==q->next->cpu_burst && t->arrival_time>q->next->arrival_time))){
                    q = q->next;
                }
                if(q!=t){
                    queue = queue->next;
                    t->next = q->next;
                    q->next = t;
                }
            }
            
        }
        else{
            if(result==NULL){
                result = create_gantt();
                end = result;
            }
            else{
                gantt* temp = create_gantt();
                end->next = temp;
                end = temp;
            }
        }
        time++;
    }
    print_chart(result,(float)sum_turn/pcnt,(float)sum_wait/pcnt);   
    reset();
}

int main(){
    printf("--------SCHEDULING ALGORITHMS--------\n");
    int loop = 1;
    while(loop){
        printf("\nChoose the scheduling algorithm:-\n");
        printf("1. First-Come First-Served\n");
        printf("2. Shortest Job First\n");
        printf("3. Round Robin Scheduling\n");
        printf("4. Priority Scheduling\n");
        printf("5. Shortest Remaining Time First\n");
        printf("6. Longest remaining time first\n");
        printf("7. Exit\n");
        int ch;
        scanf("%d", &ch);
        switch (ch)
        {
        case 1:
            fcfs();
            break;
        case 2:
            sjf();
            break;
        case 3:
            rr();
            break;
        case 4:
            ps();
            break;
        case 5:
            srtf();
            break;
        case 6:
            lrtf();
            break;
        case 7:
            printf("-------Thank You-------\n");
            loop = 0;
            break;
        default:
            printf("Invalid Input!\n");
        }               
    }
}