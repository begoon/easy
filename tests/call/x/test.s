void a(int x){
    printf("%s\n", concat(2, "a(): ", str(x)));
}
int main()
{
    a(100);
    exit(0);
}
