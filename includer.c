#include <stdio.h>
int main (int argc, char *argv[]) {


        FILE *fl;
        FILE *sf;
        int ch,x,sfch;
        int intag,instarttag,inendtag,specialch;
        int includepathisnext;
        char cpystring[2];
        char starttag[200];
        char endtag[200];
        char includepath[200];
        char sysstring[200];
        int debuglevel;

        //check that program has been called correctly, only one parameter in
        //addition to the program name.
        if (argc!=2) {
                printf("\nerror requires an argument\n");
                return 1;
        };

        //open file for reading
        fl=fopen(argv[1],"r");
        includepathisnext=0;
        instarttag=0;
        inendtag=0;
        specialch=0;
        //debuglevel=1;   //set this to 1 for more diagnostic info
        debuglevel=0;
        strcpy((char *)&starttag,"");
        strcpy((char *)&endtag,"");
        strcpy((char *)&includepath,"");
        strcpy((char *)&sysstring,"");
        strcpy((char *)&cpystring,"");




        if (debuglevel>0) {
                printf("\nFILE TO PARSE IS %s\n",argv[1]);
        };


        //repeat until end of file
        while (!feof(fl)) {
                ch=fgetc(fl);
                specialch=0;

                //start of a tag detected ...
                if (ch=='<') {
                        intag=1;
                        specialch=1;
                        instarttag=1;
                        inendtag=0;
                        strcpy((char *)&starttag,"");
                        strcpy((char *)&endtag,"");
                        includepathisnext=0;
                };

                //aha ... a closing tag.
                if ((ch=='/') && (intag==1)) {
                        inendtag=1;
                        instarttag=0;
                        specialch=1;
                };

                //end of a tag detected
                if (ch=='>') {
                        specialch=1;

                        //were we in a tag anyway?
                        if (intag==1) {

                                //was it a start tag?
                                if (instarttag==1) {
                                        //check for include start....
                                        if (strcmp((char *)&starttag,"include")==0) {
                                                //include found, set flags so that from now on, the path is stored...
                                                includepathisnext=1;
                                                if (debuglevel>0) {
                                                        printf("\nINCLUDE-ELEMENT-START-TAG FOUND\n");
                                                };
                                        } else {
                                                //else print the whole tag through to stdout.
                                                printf ("<%s>",&starttag);
                                                if (debuglevel>0) {
                                                        printf("\nSTART-TAG FOUND = %s\n",&starttag);
                                                };
                                        };
                                        //empty the starttag string
                                        strcpy((char *)&starttag,"");
                                }

                                //was it an end tag?
                                if (inendtag==1) {
                                        //check it's the right type of tag...
                                        if (strcmp((char *)&endtag,"include")==0) {
                                                //include found
                                                if (debuglevel>0) {
                                                       printf("\nEND-TAG OF INCLUDE ELEMENT FOUND INCLUDE PATH=%s\n",&includepath);
                                                };
                                                includepathisnext=0;
                                                //execute include
                                                sprintf((char *)&sysstring,"includer /repository/%s.xml",&includepath);
                                                if (debuglevel>0) {
                                                       printf("\nEXECUTING %s\n",&sysstring);
                                                };
                                                sf=popen(sysstring,"r"); //this is the same as system, except returns handle to pipe.
                                                while (feof(sf)==0) {
                                                        sfch=fgetc(sf);
                                                        if (sfch!='ÿ') {
                                                                putchar(sfch);
                                                        };
                                                };
                                                pclose(sf);
                                                strcpy((char *)&includepath,"");

                                        //else print the tag through to stdout..
                                        } else {
                                                printf ("</%s>",&endtag);
                                                if (debuglevel>0) {
                                                       printf("\nEND-TAG OF ELEMENT FOUND = %s\n",&endtag);
                                                };
                                        };
                                        //empty end-tag string
                                        strcpy((char *)&endtag,"");
                                };

                                intag=0;
                                instarttag=0;
                                inendtag=0;
                                specialch=1;
                        } else {
                                printf("\nParse Error '>' detected when not in tag!\n");
                        };
                };   //if  ch=='>'


                //create a string for concatenation based on the current character
                cpystring[0]=ch;
                cpystring[1]=0;

                if ((ch!='<') && (ch!='>') && (ch!='/') && (ch!='ÿ')) {
                        if (instarttag==1) {
                                strcat((char *)&starttag,(char *) &cpystring);
                        };

                        if (inendtag==1) {
                                strcat((char *)&endtag,(char *)&cpystring);
                        };

                        if (includepathisnext==1) {
                                strcat((char *)&includepath,(char *)&cpystring);
                        };


                        if (specialch==0) {
                                if (intag==0) {
                                        if (includepathisnext==0) {
                                                putchar(ch);
                                        };
                                };
                        };
                };

        };//end of main loop  (repeat until end of file)

        //close file being read
        fclose(fl);

};  //end of program
