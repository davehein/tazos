void main(void)
{
    int i;

    for (i = 1; i <= 10; i++)
    {
        msleep(500);
        printf("Hello World - %d\n", i);
    }
    printf("Goodbye\n");
}
