/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * This program demonstrates the use of the merge sort algorithm.  For
 * more information about this and other sorting algorithms, see
 * http://linux.wku.edu/~lamonml/kb.html
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define NUM_ITEMS 10

void mergeSort(int numbers[], int temp[], int array_size);
void m_sort(int numbers[], int temp[], int left, int right);
void merge(int numbers[], int temp[], int left, int mid, int right);

int numbers[NUM_ITEMS];
int temp[NUM_ITEMS];


int main()
{
  int i, j;
  int fd[2];
  int pid, status;

  //seed random number generator
  srand(getpid());

  //fill array with random integers
  for (i = 0; i < NUM_ITEMS; i++) numbers[i] = rand();

  if (pipe(fd) == -1) {
    perror("pipe failed.");
    exit(1);
  }

  if ((pid=fork())== -1) {
    perror("fork failed.");
    exit(1);
  }

  if(pid == 0) {
    close(fd[0]);
    for(i=0;i<(NUM_ITEMS/2 + NUM_ITEMS%2);i++) {  //後ろ半分を前に持ってくる
      numbers[i] = numbers[i+NUM_ITEMS/2];
    }
    mergeSort(numbers, temp, NUM_ITEMS/2 + NUM_ITEMS%2);
    for(j=0;j<(NUM_ITEMS/2 + NUM_ITEMS%2);j++) { //親プロセスに送信
      if(write(fd[1], &numbers[j], sizeof(int)) == -1) {
        perror("pipe write.");
        exit(1);
      }
    }
    exit(0);
  }
  else {
    close(fd[1]);
    mergeSort(numbers, temp, NUM_ITEMS/2);
    for(j=0;j<(NUM_ITEMS/2 + NUM_ITEMS%2);j++) {  //子プロセスから受信
      if(read(fd[0], &temp[j], sizeof(int)) == -1) {
        perror("pipe read.");
        exit(1);
      }
    }
    for(i=0;i<(NUM_ITEMS/2 + NUM_ITEMS%2);i++) {
      numbers[i+NUM_ITEMS/2] = temp[i];
    }
    merge(numbers, temp, 0, NUM_ITEMS/2, NUM_ITEMS-1);
  }

  //perform merge sort on array


  printf("Done with sort.\n");

  for (i = 0; i < NUM_ITEMS; i++) {
    printf("%i\n", numbers[i]);
  }

  return 0;
}


void mergeSort(int numbers[], int temp[], int array_size)
{
  m_sort(numbers, temp, 0, array_size - 1);
}



void m_sort(int numbers[], int temp[], int left, int right)
{
  int mid;

  if (right > left)
  {
    mid = (right + left) / 2;
    m_sort(numbers, temp, left, mid);
    m_sort(numbers, temp, mid+1, right);

    merge(numbers, temp, left, mid+1, right);
  }
}


void merge(int numbers[], int temp[], int left, int mid, int right)
{
  int i, left_end, num_elements, tmp_pos;

  left_end = mid - 1;
  tmp_pos = left;
  num_elements = right - left + 1;

  while ((left <= left_end) && (mid <= right))
  {
    if (numbers[left] <= numbers[mid])
    {
      temp[tmp_pos] = numbers[left];
      tmp_pos = tmp_pos + 1;
      left = left +1;
    }
    else
    {
      temp[tmp_pos] = numbers[mid];
      tmp_pos = tmp_pos + 1;
      mid = mid + 1;
    }
  }

  while (left <= left_end)
  {
    temp[tmp_pos] = numbers[left];
    left = left + 1;
    tmp_pos = tmp_pos + 1;
  }
  while (mid <= right)
  {
    temp[tmp_pos] = numbers[mid];
    mid = mid + 1;
    tmp_pos = tmp_pos + 1;
  }

  for (i=0; i <= num_elements; i++)
  {
    numbers[right] = temp[right];
    right = right - 1;
  }
}
