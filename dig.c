#include <stdio.h>

int main(){
    int n,i,j,aux;
    scanf("%d",&n);
    int V[n][n];
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            scanf("%d",&V[i][j]);
        }
    }
    for(i=0;i<n;i++){
        aux = V[i][i]; 
        V[i][i]=V[i][n-i-1];
        V[i][n-i-1] = aux;
    }

    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            printf("%d ",V[i][j]);
        }
        printf("\n");
    }
    return 0;
}