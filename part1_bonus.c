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
#include <asm/pgtable.h>
#include <asm/mmu_context.h>

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
int hundred_present_count = 0;
int hundred_swapped_count = 0;
int hundred_invalid_count = 0;

int hundred_present_start = 0;
int hundred_present_end = 0;
int hundred_swapped_start = 0;
int hundred_swapped_end = 0;
int hundred_invalid_start = 0;
int hundred_invalid_end = 0;

static int proj3_pt1_init(void){

	printk("pid= %d", mypid);//prints out the first line of the module and shows the given PID which was the parameter

	for_each_process(task){//looks for processes with task struct and matching PID
		if(task->pid == mypid){
			goto found;//so we found it, now we gotta find the pages
		}
	}

	found:
		vma = task->mm->mmap;
		while(vma){
			for(_i = vma->vm_start; _i < vma->vm_end;_i+= PAGE_SIZE){
				pgd = pgd_offset(task->mm, _i);
				p4d = p4d_offset(pgd, _i);
				pud = pud_offset(p4d, _i);
				pmd = pmd_offset(pud, _i);
				pte = pte_offset_map(pmd, _i);

				down_read(&task->mm->mmap_sem);

				if(!pte_none(*pte)){
					if(pte_present(*pte)){
						present_pages++;
					}
					else{
						swapped_pages++;
					}
				}
				if(!pte_none(*pte) && pte_present(*pte)){ //it is valid/present in page table
					hundred_present_count++;	//count for # of present
					hundred_swapped_count=0;
					hundred_invalid_count=0;
				}
				else if(!pte_none(*pte) && !pte_present(*pte)){ //it is valid/swapped in page table
					hundred_swapped_count++;	//count for # of swapped
					hundred_present_count=0;
					hundred_invalid_count=0;
				}
				else{					//it is invalid in page table not in physical memory
					hundred_invalid_count++;	//count for # of invalids
					hundred_present_count=0;
					hundred_swapped_count=0;
				}

				if(hundred_present_count == 1){
					 hundred_present_start = _i;
				}
				if(hundred_present_count == 100){
					 hundred_present_end = _i;
					 printk("PRESENT START ADDRESS %u END ADDRESS %u",hundred_present_start,hundred_present_end);
				}
                                if(hundred_swapped_count == 1){
                                         hundred_swapped_start = _i;
				}
                                if(hundred_swapped_count == 100){
                                         hundred_swapped_end = _i;
                                         printk("SWAPPED START ADDRESS %u END ADDRESS %u",hundred_swapped_start,hundred_swapped_end);
                                }
                                if(hundred_invalid_count == 1){
                                         hundred_invalid_start = _i;
				}
                                if(hundred_invalid_count == 100){
                                         hundred_invalid_end = _i;
                                         printk("INVALID START ADDRESS %u END ADDRESS %u",hundred_invalid_start,hundred_invalid_end);
                                }

				up_read(&task->mm->mmap_sem);
			}
			vma = vma->vm_next;
		}

	printk("PRESENT: %d SWAPPED: %d", present_pages, swapped_pages);
	return 0;
}

static void proj3_pt1_exit(void){
	printk("Goodbye");
}

module_init(proj3_pt1_init);
module_exit(proj3_pt1_exit);
