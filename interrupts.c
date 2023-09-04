#include "interrupts.h"



void interrupt_handler(unsigned int cause){
        if(cause & LINE_0_MASK){
            /*IPI*/
            //ignore
        } 
        else if(cause & LINE_1_MASK){
            plt(BIOSDATAPAGE);
        }
        else if(cause & LINE_2_MASK){
            timer_interrupt();
        }
        else if(cause & LINE_3_MASK){
            non_timer_interrupts(3);
        }
        else if(cause & LINE_4_MASK){
            non_timer_interrupts(4);
        }
        else if(cause & LINE_5_MASK){
            non_timer_interrupts(5);
        }
        else if(cause & LINE_6_MASK){
            non_timer_interrupts(6);
        }
        else if(cause & LINE_7_MASK){
            terminal_handling();
        }
}


void plt(state_t *processor_state)
{
    setTIMER(5); //da testare il valore
    //copy the processor state at the time of the exception into che current process's pcb
    current_proc->p_s = *processor_state;
    insertProcQ(&(ready_list), current_proc); //insert the process in the ready queue
    //scheduler(); call the scheduler
}


void non_timer_interrupts(int line) {
    unsigned int bitmap = CDEV_BITMAP_ADDR(line); 

    for (int dev_number = 1; dev_number < 9; dev_number++) {
        // Creare una maschera specifica per il dispositivo corrente
        unsigned int device_mask = 1 << dev_number;

        // Verifico se il dispositivo corrente ha causato un interrupt
        if (bitmap & device_mask) { 
            // Calculate the address for this device's device register.
            dtpreg_t *dev_reg = DEV_REG_ADDR(line, dev_number);

            // Save off the status code from the device's device register
            unsigned int status = dev_reg->status;

            // Acknowledge the outstanding interrupt
            dev_reg->command = ACK;

            // Perform a V operation on the Nucleus maintained semaphore associated with this device
            int sem_index = (line - 3) * 8 + dev_number; // i device sono solo sulle linee da 3 a 7 ed ognuna ne ha 8
            device_sem[sem_index]++;
            pcb_t *proc = headBlocked(&(device_sem[sem_index])); //prendo il primo processo bloccato sul semaforo

            if (proc != NULL) {
                proc->p_semAdd = NULL; //resetto il semaforo
                proc->p_s.reg_v0 = status;
                insertProcQ(ready_list, outBlocked(proc)); //inserisco il processo nella ready list
                device_sem[sem_index]--;
            }
        }
    }

    if (current_proc == NULL) 
        scheduler(); //lo scheduler fa una wait
    else
        LDST(BIOSDATAPAGE);
}

void timer_interrupt()
{
    //loading the Interval Timer with a new value: 100 milliseconds
    LDIT(100);

    //Unblock ALL pcbs blocked on the Pseudo-clock semaphore. 
    pcb_t *proc = headBlocked(&(device_sem[0]));
    while( proc != NULL){
        proc->p_semAdd = NULL; //resetto il semaforo
        insertProcQ(ready_list, outBlocked(proc)); //inserisco il processo nella ready list
        proc = headBlocked(&(device_sem[0]));
    }
    //Reset the Pseudo-clock semaphore to zero
    device_sem[0] = 0; 

    if(current_proc == NULL) scheduler(); //lo scheduler fa una wait
    else LDST(BIOSDATAPAGE); 
}

void terminal_handling()
{
   
   for(int dev_number = 1; dev_number < 9; dev_number++){
        //Calculate the address for this device’s device register.
        termreg_t *dev_reg = DEV_REG_ADDR(7, dev_number);
        unsigned int status = 0;
        //assumo che i primi 4 device siano per la trasmissione e gli altri per la ricezione dato che la trasmissione ha priorità maggiore
        if(dev_number > 0 && dev_number <= 4){
            status = dev_reg->transm_status; 
            dev_reg->transm_command = ACK;
        } else {
            status = dev_reg -> recv_status;
            dev_reg->recv_command = ACK;
        }


        //Perform a V operation on the Nucleus maintained semaphore associated with this (sub)device
        int sem_index = 4 * 8 + dev_number; //i device sono solo sulle linee da 3 a 7 ed ognuna ne ha 8
        device_sem[sem_index]++; //incremento il semaforo
        pcb_t  *proc = headBlocked(&(device_sem[sem_index]));  //prendo il primo processo bloccato sul semaforo

        if(proc != NULL){
            proc->p_semAdd = NULL; //resetto il semaforo
            proc->p_s.reg_v0 = status;
            insertProcQ(ready_list, outBlocked(proc)); //inserisco il processo nella ready list
            device_sem[sem_index]--; 
        }
   }
    
    
   if(current_proc == NULL)
         scheduler(); //lo scheduler fa una wait
   else LDST(BIOSDATAPAGE); 
}


