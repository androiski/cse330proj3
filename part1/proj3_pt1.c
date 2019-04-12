#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/mm_types.h>

#include <linux/kernel_stat.h>
#include <linux/mm.h>
#include <linux/sched/mm.h>
#include <linux/sched/signal.h>

/*
 * takes in int as a parameter for the module
 * e.g. "insmod ./proj3_pt1.ko mypid=99"
 */
static int  mypid = 0;
module_param(mypid,int, 0);


struct task_struct *task;
//struct mm_struct *mm;
struct vm_area_struct *vma;

/*
 * 5-layer page table goes PGD->P4D->PUD->PMD->PTE
 */
pgd_t *pgd;
p4d_t *p4d;
pud_t *pud;
pmd_t *pmd;
pte_t *pte;

int _i;
int present_pages = 0;//number of pages in physical memory
int swapped_pages = 0;//number of pages swapped out



static int proj3_pt1_init(void){
	
	printk(KERN_WARNING "pid= %d", mypid);//prints out the first line of the module and shows the given PID which was the parameter

	for_each_process(task){//looks for processes with task struct and matching PID
		if(task->pid == mypid){
			goto found;//so we found it, now we gotta find the pages
		}
	}

	found:
		vma = task->mm->mmap;
		while(vma){
			for(_i = vma->vm_start; _i <= vma->vm_end; _i++){
				pgd = pgd_offset(task->mm, _i);
				p4d = p4d_offset(pgd, _i);
				pud - pud_offset(p4d, _i);
				pmd = pmd_offset(pud, _i);
				pte = pte_offset_map(pmd, _i);
				
				down_read(&task->mm->mmap_sem);

				if(pte_present(*pte)){
					present_pages++;		
				}
				else{
					swapped_pages++;
				}

				up_read(&task->mm->mmap_sem);
			}
			vma = vma->vm_next;
		}
		printk(KERN_WARNING "PRESENT: %d SWAPPED: %d", present_pages, swapped_pages);
	return 0;
}

static void proj3_pt1_exit(void){
	printk(KERN_WARNING "Goodbye");
}

module_init(proj3_pt1_init);
module_exit(proj3_pt1_exit);



