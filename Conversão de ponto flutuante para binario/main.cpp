#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <string.h>
#include "resource.h"
#include<math.h>

typedef struct Dados_maquina{
    int uper=0;
    int lower=0;
    int precisao=0;
    double decimal=0;
    int mantissa[64]={0};
    char Cmantissa[100];
    char normatizada[100];
    int expoente=0;
    int sinal=0;
    char erros[100];
    char complemento1[100];
    char complemento2[100];
    char Decimal_final[100];
    int ponto_flutuante;
    int control=0;
};

void Decimal(Dados_maquina *maquina){
    char Char_inteiro[100],Char_decimal[100], sinal=0;
    double fracao=0;
    int aux, inteiro=0, i=0, j=0;
    int tam=maquina->ponto_flutuante;

    if(maquina->sinal !=1 ){///Converte sinal para caracter
        sinal='+';
    }else{
        sinal='-';
    }

    for(aux=0; aux<(maquina->ponto_flutuante); aux++){///inteiro para string
        Char_inteiro[aux]=maquina->Cmantissa[aux];
    }
    Char_inteiro[aux]='\0';

    for(aux=maquina->ponto_flutuante, j=0; aux<(maquina->precisao); aux++){///Decimal para string
        Char_decimal[j]=maquina->Cmantissa[aux];
        j++;
    }
    Char_decimal[j]='\0';

    while( tam-- ) {///Convert inteiro
        if( Char_inteiro[tam] == '0' || Char_inteiro[tam] == '1' ) {
            inteiro = inteiro + pow(2, i++) * (Char_inteiro[tam] - '0');
        }
    };

    i=0;
    double virgula=2;
    while(i!=strlen(Char_decimal)){///Converte Decimal
        if(Char_decimal[i]=='1'){
            fracao=fracao+(1/virgula);
        }
        printf("\n%f", fracao);
        virgula=virgula*2;
        i++;
    }
    fracao+=inteiro;
    sprintf(maquina->Decimal_final,"%c%.19lf",sinal, fracao);
}

void complementos(Dados_maquina *maquina){
    int aux;
    for(aux=0; aux<maquina->control; aux++){///Complemento de 1
        if(maquina->complemento1[aux]=='1'){
            maquina->complemento1[aux]='0';
        }else{
            maquina->complemento1[aux]='1';
        }
    }
    char auxC[64];
    strcpy(auxC, maquina->complemento1);
    strcpy(maquina->complemento2, maquina->complemento1);///Salvo o complemento de 2
    sprintf(maquina->complemento1,"%s %d %sx2^%d",maquina->erros,maquina->sinal, auxC, maquina->expoente);



    if(maquina->complemento2[maquina->control-1]=='0'){///Complemento de 2
        maquina->complemento2[maquina->control-1]='1';
    }else{
        for(aux=maquina->control-1; aux>=0; aux--){
            if(maquina->complemento2[aux]=='1'){
                maquina->complemento2[aux]='0';
            }else{
               maquina->complemento2[aux]='0';
            }
            if(maquina->complemento2[aux-1]=='0'){
                maquina->complemento2[aux-1]='1';
                break;
            }
        }
    }
    strcpy(auxC, maquina->complemento2);
    sprintf(maquina->complemento2,"%s %d %sx2^%d",maquina->erros,maquina->sinal, auxC, maquina->expoente);
}

void normatiza(Dados_maquina *maquina){
    int aux=0, i=0;

    for(aux=0; aux<64; NULL){///Transforma a mantissa em string
        maquina->Cmantissa[aux]=maquina->mantissa[aux]+'0';
        aux++;
    }
    maquina->Cmantissa[aux]='\0';

    if(maquina->expoente <= 0){///Transforma a normatizada em string com expoente negativo
        int posicao=maquina->expoente*-1;
        for(aux=posicao, i=0; i<maquina->precisao; aux++){
            if(maquina->mantissa[aux]==1 || maquina->mantissa[aux]==0){
                maquina->normatizada[i]=maquina->mantissa[aux]+'0';
                i++;
            }else{
                break;///Tranca para não acessar bits nulos
            }
        }
        maquina->control=maquina->precisao*-1;
        maquina->normatizada[i]='\0';

    }else{
        int control=maquina->expoente;
        for(aux=maquina->expoente; aux<(maquina->precisao); aux++){/// Realiza o controle da normatização
            if(maquina->mantissa[aux]==1){
                control=maquina->precisao;
                break;
            }
        }
        if(maquina->precisao<control){
            control=maquina->precisao;
        }
        for(aux=0; aux<control; aux++){///Transforma a normatizada em string com expoente positivo
            maquina->normatizada[aux]=maquina->mantissa[aux]+'0';
        }
        maquina->normatizada[aux]='\0';
        maquina->control=control;///Corrige a normatização
    }
    char auxC[64];
    strcpy(auxC, maquina->normatizada);
    strcpy(maquina->complemento1, maquina->normatizada);///Atribui o valor ao complemento antes de mudar a string
    sprintf(maquina->normatizada,"%s %d %sx2^%d",maquina->erros,maquina->sinal, auxC, maquina->expoente);/// Add o expoente a string e o bit de sinal
}

void erros(Dados_maquina *maquina){
    char overflow[20]={' '}, underflow[20]={' '}, truncamento[20]={' '};

    for(int aux=maquina->precisao; aux<64; aux++){
        if(maquina->mantissa[aux]==1){/// Descubro se houve truncamento
            strcpy(truncamento,"truncamento");
        }
    }

    if((maquina->expoente) > (maquina->uper) && maquina->uper!=0){
        strcpy(overflow,"overflow");
    }
    if(maquina->expoente < maquina->lower && maquina->lower!=0){
        strcpy(underflow,"underflow");
    }
    sprintf(maquina->erros,"%s %s %s", truncamento, overflow, underflow);
}

void binario(Dados_maquina *maquina){
    if(maquina->decimal<0){
        maquina->sinal=1;///Salva o sinal de amplitude
        maquina->decimal*=-1;/// transforma em positivo o numero que estiver em negativo para a Conv. em binario
    }
    int inteiro=maquina->decimal;
    double decimal=maquina->decimal-inteiro;
    int bin[64]={0};

    if(inteiro!=0){
        for (int aux = 63; inteiro!=0; aux--){//// Inteiro para decimal
              if (inteiro % 2 == 0){
                bin[aux] = 0;
              }
              else{
                bin[aux] = 1;
              }
              inteiro = inteiro / 2;
              maquina->expoente++;
              maquina->ponto_flutuante++;///Salva a virgula do ponto flutuante
        }
        for(int aux=0, i=0; aux<64; aux++){////// Adiciona o inteiro a mantissa e remove as sobras da frente do numero
            if(bin[aux]==1 || i!=0){
                maquina->mantissa[i]=bin[aux];
                i++;
            }
        }
    }


    if(decimal!=0){
        double x=decimal;
        int i=maquina->expoente;

        while(i<64){
            x=x*2;
            if(x==1){
                maquina->mantissa[i]=1;
                break;
            }
            else if(x<1){
                maquina->mantissa[i]=0;
            }
            else if(x>1){
                maquina->mantissa[i]=1;
                x=x-1;
            }
            i++;
        }

        if(maquina->expoente==0){
            int p=0;///Posiçao do ultimo valor 1
            for(int aux=0; aux<64; aux++){
                if(maquina->mantissa[aux]==0){
                    maquina->expoente--;
                }else{
                    break;
                }
            }
        }
    }
}

HINSTANCE hInst;

Dados_maquina maquina;

BOOL CALLBACK DlgMain(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch(uMsg){

            case WM_INITDIALOG:
            {
                system("cls");
            }
            return TRUE;

            case WM_CLOSE:
            {
                EndDialog(hwndDlg, 0);
            }
            return TRUE;

            case WM_COMMAND:
            {
                switch(LOWORD(wParam)){

                    case cad_calcula:{
                        system("cls");
                        memset(&maquina, 0, sizeof maquina);///Funcão para limpar struct, e apagar lixo de memoria
                        SetDlgItemText(hwndDlg,ent_comp1, "aquarde...");
                        SetDlgItemText(hwndDlg,ent_comp2, "aquarde...");
                        SetDlgItemText(hwndDlg,ent_bin, "aquarde...");
                        SetDlgItemText(hwndDlg,ent_Normatizado, "aquarde...");
                        SetDlgItemText(hwndDlg,ent_ConvDec, "aquarde...");

                        maquina.precisao=GetDlgItemInt(hwndDlg,ent_precisao, NULL, FALSE);
                        maquina.uper=GetDlgItemInt(hwndDlg,ent_uper, NULL, FALSE);
                        maquina.lower=GetDlgItemInt(hwndDlg,ent_lower, NULL, FALSE);


                        if(maquina.precisao<=64 && maquina.precisao>=0 && maquina.lower<=maquina.uper){
                            char string[100000];
                            GetDlgItemText(hwndDlg, ent_decimal, string, sizeof(string));
                            maquina.decimal=atof(string);
                            double decimal=maquina.decimal;
                            binario(&maquina);
                            erros(&maquina);
                            normatiza(&maquina);
                            complementos(&maquina);
                            Decimal(&maquina);

                            SetDlgItemText(hwndDlg,ent_bin, maquina.Cmantissa);
                            SetDlgItemText(hwndDlg,ent_Normatizado, maquina.normatizada);
                            SetDlgItemText(hwndDlg,ent_ConvDec, maquina.Decimal_final);

                            if(decimal < 0){
                                SetDlgItemText(hwndDlg,ent_comp1, maquina.complemento1);
                                SetDlgItemText(hwndDlg,ent_comp2, maquina.complemento2);

                            }

                        }
                        if(maquina.precisao>=64 || maquina.precisao<=0){
                            SetDlgItemText(hwndDlg,ent_comp1, "erro nos parametros da precisao...");
                            SetDlgItemText(hwndDlg,ent_comp2, "erro nos parametros da precisao...");
                            SetDlgItemText(hwndDlg,ent_bin, "erro nos parametros da precisao...");
                            SetDlgItemText(hwndDlg,ent_Normatizado, "erro nos parametros da precisao...");
                            SetDlgItemText(hwndDlg,ent_ConvDec, "erro nos parametros da precisao...");
                        }
                        if(maquina.decimal==0){
                            SetDlgItemText(hwndDlg,ent_comp1, "Não é possivel sem o valor do decimal...");
                            SetDlgItemText(hwndDlg,ent_comp2, "Não é possivel sem o valor do decimal...");
                            SetDlgItemText(hwndDlg,ent_bin, "Não é possivel sem o valor do decimal...");
                            SetDlgItemText(hwndDlg,ent_Normatizado, "Não é possivel sem o valor do decimal...");
                            SetDlgItemText(hwndDlg,ent_ConvDec, "Não é possivel sem o valor do decimal...");
                        }
                        if(maquina.lower>maquina.uper){
                            SetDlgItemText(hwndDlg,ent_comp1, "erro nos parametros do lower e uper...");
                            SetDlgItemText(hwndDlg,ent_comp2, "erro nos parametros do lower e uper...");
                            SetDlgItemText(hwndDlg,ent_bin, "erro nos parametros do lower e uper...");
                            SetDlgItemText(hwndDlg,ent_Normatizado, "erro nos parametros do lower e uper...");
                            SetDlgItemText(hwndDlg,ent_ConvDec, "erro nos parametros do lower e uper...");
                        }

                    }
                }
        }
    return TRUE;
    }
    return FALSE;
}
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    hInst=hInstance;
    InitCommonControls();
    return DialogBox(hInst, MAKEINTRESOURCE(DLG_MAIN), NULL, (DLGPROC)DlgMain);
}


