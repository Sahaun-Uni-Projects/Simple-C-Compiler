// A program fragment
float   x1 =   3.125;
int a = 5;
/* Definition of the

function f1 */
double f1(int    x)
{
  double   z = 0.1;
  if (x < x1) z = 0.01;
    else if (x == 1)  z = 0.001;
    else return z;
  return 0;
}

//* Beginning of 'main'
int main(void)
{
  int n1; double z;
    n1=25; z=f1();
}