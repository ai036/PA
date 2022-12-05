#include <stdio.h>
#include <assert.h>

int main() {
  FILE *fp = fopen("/share/files/num", "r+");
  assert(fp);
int n;
  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  assert(size == 5000);


  fseek(fp, 0, SEEK_SET);
  for (int i = 0; i < 500; i ++) {
    fscanf(fp, "%4d", &n);
    printf("%dth ",i);

    	printf("n:%d\n",n);

  }

  fclose(fp);

  printf("PASS!!!\n");

  return 0;
}
