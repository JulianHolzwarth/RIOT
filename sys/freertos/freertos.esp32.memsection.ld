
SECTIONS
{
    .iram0.text :
  {
    *portable.o
    *queue.o
    *semphr.o
    *task.o
    *timers.o
  } > iram0_0_seg
}