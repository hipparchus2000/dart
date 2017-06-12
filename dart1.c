#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main (int argc, char *argv[]) {

  char values[2000];
  char buf[2000];
  char *title="Document Atom Repository and Transformation";
  int i=0;
  char *called_uri;
  char *envstr;
  const char *envvar="REQUEST_URI";
  char *nxt;
  char fieldname[20][200];
  char fieldvalue[20][400];
  int continue_flag;
  int parameter_count=0;
  FILE *infile;
  char x;    
  unsigned int rannum;     //random number for numbering objects as placed in database
  int command_param;
  int username_param;
  int password_param;
  int objtype_param;
  int title_param;
  int shresult;
  int filename_param;
  char *extension;
  char cmd_line[200];
  char new_object_name[20];
  int debug=0;

  //seed the random number generator
  srand (time(0));

  printf ("Content-type: text/html\n\n");
  printf ("<html>\n");
  printf ("   <head>\n");
  printf ("      <title>%s</title>\n",title);
  printf ("   </head>\n");
  printf ("   <body>\n");
  printf ("     <h1>%s<h1>\n<br>",title);

  //printf ("    Number of arguments passed=%d<br>",argc);  
  //  only useful for script with ?param1+param2+param3
  //for (i=0;i<argc;i++) {
  //  printf("argument %d = %s<br>",i,argv[i]);
  //} 

  called_uri=(char *)malloc(2048);
  envstr=getenv(envvar);
  if (envstr!=NULL) {
    strcpy(called_uri,envstr);
    if (debug!=0) {
      printf ("   uri = %s<br>\n",called_uri);
    };
  };

  //uri processing: discard end 'Submit=Submit'
  //replace %0D%0A with crlf
  //in text area, lines are seperated (note above crlf) with
  //  '+'
  // fieldname=value
  // value pairs are seperated by '&'
  //
  //ignore start string '/cgi-bin/dart?'

  //char *strtok(char *s, const char *delim)
  //first call should have s as first argument
  //subsequent calls whould have first arg=NULL
  //token can change between calls
  
  //get everything up to the first ?
  nxt=strtok(called_uri, "?");
  i=0;
  continue_flag=1;

  while (continue_flag==1) {

    //get first field
    nxt=strtok(NULL,"=");
    if (nxt!=NULL) {
      strcpy(&fieldname[i][0],nxt);
 
      //get first value
      nxt=strtok(NULL,"&");
      if (nxt!=NULL) {
        strcpy(&fieldvalue[i][0],nxt);
  
        //note value must be string replaced to get 0D and 0A to proper
        //characters and + replaced with ' '
  
        if (debug!=0) {
          printf("field name %d  =%s\n<br>",i,&fieldname[i][0]);
          printf("field value %d =%s\n<br>",i,&fieldvalue[i][0]);
        };
        i++;
      } else {
        continue_flag=0;
      };
    } else {
      continue_flag=0;
    };
  };

  parameter_count=i;

  // parse the parameters. command gives form to process
  command_param=-1;
  username_param=-1;
  password_param=-1;

  for ( i=0 ; i<parameter_count ; i++) {
    if (strcmp("command",&fieldname[i][0])==0) {
      command_param=i;
    };    
    if (strcmp("username",&fieldname[i][0])==0) {
      username_param=i;
    };    
    if (strcmp("password",&fieldname[i][0])==0) {
      password_param=i;
    };    
  }; 

  if (username_param==-1) {
    command_param=-1;
  };

  if (password_param==-1) {
    command_param=-1;
  };

  //if there are no command parameters, give the user a login prompt
  if (command_param==-1) {  
    //login
    //generates a form with fields username and password
    printf( "<form action=\"dart\">\n");
    printf( "<h2>Login Page</h2><br>");
    printf( "Please enter username and password, then press 'Submit'<br>\n" );
    printf( "<input type=\"hidden\" name=\"command\" value=\"menu\">" );
    printf( "<table border=\"2\"><tr><td>Username:</td>\n" );
    printf( "  <td><input type=\"text\" name=\"username\" ></td></tr>" );
    printf( "<tr><td>Password:</td>\n" );
    printf( "  <td><input type=\"password\" name=\"password\" ></td></tr></table>" );
    printf( "  <br><input type=\"Submit\" name=\"Submit\"></form>" );
    printf ("   </body>\n");
    printf ("</html>\n");
    return 0;
  };

  if (strcmp(&fieldvalue[command_param][0],"menu")==0) {

    //submit button on login form causes submission to "menu" which
    //then generates a list of options based on username, password and group membership.

    //look up username
    //shell ("mysql -B -N  -e \"USE jeff; SELECT * FROM mytable;\");   //add -H to get HTML output
    //shell ("mysql -e "USE jeff; INSERT INTO mytable VALUES ('123',456');");

    printf( "<h2>User Menu</h2><br>");
    
    printf ("<form action=\"dart\">\n");
    printf ("Logged in as: %s<br><br>",fieldvalue[username_param]);
    printf ("<input type=\"hidden\" name=\"username\" value=\"%s\">\n",fieldvalue[username_param]);
    printf ("<input type=\"hidden\" name=\"password\" value=\"%s\">\n",fieldvalue[password_param]);
    printf ("<input type=\"submit\" name=\"submit\" value=\"Submit Document Object\">\n");
    printf ("<input type=\"hidden\" name=\"command\" value=\"submitdocumentobject\">\n");
    printf ("</form><br>\n" );


    printf ("<form action=\"dart\">\n");
    printf ("<input type=\"hidden\" name=\"username\" value=\"%s\">\n",fieldvalue[username_param]);
    printf ("<input type=\"hidden\" name=\"password\" value=\"%s\">\n",fieldvalue[password_param]);
    printf ("<input type=\"submit\" name=\"submit\" value=\"Search Database\">\n");
    printf ("<input type=\"hidden\" name=\"command\" value=\"searchsetup\">\n");
    printf ("</form><br>\n" );


    printf ("<form action=\"dart\">\n");
    printf ("<input type=\"submit\" name=\"submit\" value=\"Logout\">\n");
    printf ("<input type=\"hidden\" name=\"command\" value=\"logout\">\n");
    printf ("</form><br>\n" );



    printf ("   </body>\n");
    printf ("</html>\n");
    return 0;
  };

  if (strcmp(&fieldvalue[command_param][0],"submitdocumentobject")==0) {
    //submit document object
    printf( "<h2>Submit Document Object Page</h2><br>");

    printf( "<form action=\"dart\">\n" );
    printf( "<table border=\"2\">\n");
    printf ("Logged in as: %s<br><br>",fieldvalue[username_param]);
    printf ("<input type=\"hidden\" name=\"username\" value=\"%s\">\n",fieldvalue[username_param]);
    printf ("<input type=\"hidden\" name=\"password\" value=\"%s\">\n",fieldvalue[password_param]);
 
    printf( "   <tr><td>Document Object Title:</td>\n");
    printf( "   <td><input name=\"title\" type=\"text\">\n");
    printf( "   </td></tr>\n");

    printf( "   <tr><td>Repository Area to Archive in:<br>(enter nothing for default Public area)</td>\n");
    printf( "   <td><input name=\"area\" type=\"text\">\n");
    printf( "   </td></tr>\n");

    printf( "   <tr><td>Language:</td>\n");
    printf( "   <input type=\"radio\" name=\"lang\" value=\"en\">English<br>");
    printf( "   <input type=\"radio\" name=\"lang\" value=\"cy\">Welsh<br>");
    printf( "   <input type=\"radio\" name=\"lang\" value=\"fr\">French<br>");
    printf( "   <input type=\"radio\" name=\"lang\" value=\"de\">German<br>");
    printf( "   </td></tr>\n");

    printf( "   <tr><td>Filename in anonymous ftp upload area:</td>\n");
    printf( "   <td><input name=\"filename\" type=\"text\">\n");
    printf( "   </td></tr>\n");

    printf( "   <tr><td>Notes:</td>\n");
    printf( "   <td><textarea name=\"notes\" cols=\"40\" rows=\"10\"></textarea>\n");
    printf( "   </td></tr>\n");

    //note transformable is calculated from doctype definition in xml, and from that
    // the suitable xsl or whatever to use
    //printf( "   <tr><td>Document Is transformable?</td>\n");
    //printf( "   <td><input type=\"checkbox\" name=\"transformable\">\n");
    //printf( "   </td></tr>\n");

    printf( "   <tr><td>Document Type</td>\n<td>");
    printf( "   <input type=\"radio\" name=\"objtype\" value=\"xmldoc\">XML Document Object<br>");
    printf( "   <input type=\"radio\" name=\"objtype\" value=\"imagegif\">Image-GIF<br>");
    printf( "   <input type=\"radio\" name=\"objtype\" value=\"quicktimemovie\">Quicktime Movie<br>");
    printf( "   </td></tr>\n");

    printf( "   <tr><td>This document is a translation of this object<br>(enter object reference number):</td>\n");
    printf( "   <td><input name=\"translates\" type=\"text\">\n");
    printf( "   </td></tr>\n");

    printf( "   <tr><td>This document is a new version of this previous object<br>(enter object reference number):</td>\n");
    printf( "   <td><input name=\"updates\" type=\"text\">\n");
    printf( "   </td></tr>\n");

    printf( "   <tr><td>This document is a new fork of this previous object<br>(enter object reference number):</td>\n");
    printf( "   <td><input name=\"forkof\" type=\"text\">\n");
    printf( "   </td></tr>\n");

    printf( "</table>\n");
 
    printf ("<br>");
    printf ("<input type=\"submit\" name=\"submit\" value=\"Submit Document Object\">\n");
    printf ("<input type=\"hidden\" name=\"command\" value=\"objectinsertion\">\n");
    printf( "</form>\n" );

    printf ("<form action=\"dart\">\n");
    printf ("<input type=\"hidden\" name=\"username\" value=\"%s\">\n",fieldvalue[username_param]);
    printf ("<input type=\"hidden\" name=\"password\" value=\"%s\">\n",fieldvalue[password_param]);
    printf ("<input type=\"submit\" name=\"submit\" value=\"Back to menu\">\n");
    printf ("<input type=\"hidden\" name=\"command\" value=\"menu\">\n");
    printf ("</form><br>\n" );

    printf ("   </body>\n");
    printf ("</html>\n");
    return 0;
  };

  if (strcmp(&fieldvalue[command_param][0],"objectinsertion")==0) {
    //This actually inserts the object in the database, and generates a "completed ok"
    //type of message for the user.

    //parse out the fields
   title_param=-1;
   objtype_param=-1;

    for ( i=0 ; i<parameter_count ; i++) {
       if (strcmp("title",&fieldname[i][0])==0) {
          title_param=i;
       };    
       if (strcmp("objtype",&fieldname[i][0])==0) {
          objtype_param=i;
       };    
       if (strcmp("filename",&fieldname[i][0])==0) {
          filename_param=i;
       };    
    };

    if (objtype_param==-1||title_param==-1) {
         printf("<br>You have not filled in all the mandatory fields, please try again<br>");
         return(0);
    };

   //convert objtype to file_extension
   if (strcmp(&fieldvalue[objtype_param][0],"xmldoc")==0) {
        extension="xml";
   };
   if (strcmp(&fieldvalue[objtype_param][0],"imagegif")==0) {
        extension="gif";
   };
   if (strcmp(&fieldvalue[objtype_param][0],"quicktimemovie")==0) {
        extension="mov";
   };

    //generate a random number and use this as the filename
    rannum=rand();  //this is a large integer
    sprintf(new_object_name,"%d.%s",rannum,extension);
    
    //move the file from the ftp upload directory into /repository 
    sprintf(cmd_line, "mv /home/dart-inbox/%s /repository/%s",&fieldvalue[filename_param][0],new_object_name);
    printf ("moving using the commandline: %s\n<br>",&cmd_line);
    shresult=system( cmd_line );

    //insert row into relationaldatabase

    if (debug!=0) {
        printf("about to insert\n<br>");
    };

    sprintf((char *)&values,"/usr/bin/mysql  -e \"USE jeff; INSERT INTO mytable VALUES ('%s','%s');\" > x.tmp",&fieldvalue[title_param][0],new_object_name);  //&fieldvalue[objtype_param][0]
    shresult=system( values );
    infile=fopen("x.tmp","r");
    while (feof(infile)==0) {
        x=fgetc(infile);
        if (x!='ÿ') {
           putchar(x);
        };
    };
    fclose(infile);        

    if (debug!=0) {
       printf("Inserted OK\n<br>");
    };


    printf( "<h2>Document Submitted OK</h2><br>");
    printf( "If this is a transformable document, then the document will currently be\n<br>");
    printf( "transforming. You can open the document using the following link ...TBD...\n<br>");
    printf( "to check it's transformation status. Your system might be configured to send you a\n<br>");
    printf( "email upon completion of transformation\n<br>");
    printf ("Logged in as: %s<br><br>",fieldvalue[username_param]);
    printf ("<input type=\"hidden\" name=\"username\" value=\"%s\">\n",fieldvalue[username_param]);
    printf ("<input type=\"hidden\" name=\"password\" value=\"%s\">\n",fieldvalue[password_param]);
    printf ("   </body>\n");

    printf ("<form action=\"dart\">\n");
    printf ("<input type=\"hidden\" name=\"username\" value=\"%s\">\n",fieldvalue[username_param]);
    printf ("<input type=\"hidden\" name=\"password\" value=\"%s\">\n",fieldvalue[password_param]);
    printf ("<input type=\"submit\" name=\"submit\" value=\"Back to menu\">\n");
    printf ("<input type=\"hidden\" name=\"command\" value=\"menu\">\n");
    printf ("</form><br>\n" );

    printf ("</html>\n");
    return 0;
  };

  if (strcmp(&fieldvalue[command_param][0],"searchsetup")==0) {
    //search setup - form for filling by user as to what to search for is displayed
    //submit causes the search to begin
    printf( "<h2>Search Setup Page</h2><br>");
    printf( "Enter as much information as you know about the document, then press 'Search'\n<br>");

    printf( "<form action=\"dart\">\n" );
    printf( "<table border=\"2\">\n");
    printf (" Logged in as: %s<br><br>",fieldvalue[username_param]);
    printf ("<input type=\"hidden\" name=\"username\" value=\"%s\">\n",fieldvalue[username_param]);
    printf ("<input type=\"hidden\" name=\"password\" value=\"%s\">\n",fieldvalue[password_param]);
 
    printf( "   <tr><td>Document Object Title:</td>\n");
    printf( "   <td><input name=\"title\" type=\"text\">\n");
    printf( "   </td></tr>\n");

    printf( "   <tr><td>Repository Area Archived in:<br>(enter nothing for default Public area)</td>\n");
    printf( "   <td><input name=\"area\" type=\"text\">\n");
    printf( "   </td></tr>\n");

    printf( "   <tr><td>Language:</td>\n");
    printf( "   <input type=\"radio\" name=\"lang\" value=\"en\">English<br>");
    printf( "   <input type=\"radio\" name=\"lang\" value=\"cy\">Welsh<br>");
    printf( "   <input type=\"radio\" name=\"lang\" value=\"fr\">French<br>");
    printf( "   <input type=\"radio\" name=\"lang\" value=\"de\">German<br>");
    printf( "   </td></tr>\n");

    printf( "   <tr><td>Document Object Reference:</td>\n");
    printf( "   <td><input name=\"docobjref\" type=\"text\">\n");
    printf( "   </td></tr>\n");

    printf( "   <tr><td>Word to search for in 'Notes' field:</td>\n");
    printf( "   <td><input name=\"notes\" type=\"text\">\n");
    printf( "   </td></tr>\n");

    printf( "   <tr><td>Document Type</td>\n<td>");
    printf( "   <input type=\"radio\" name=\"objtype\" value=\"xmldoc\">XML Document Object<br>");
    printf( "   <input type=\"radio\" name=\"objtype\" value=\"imagegif\">Image-GIF<br>");
    printf( "   <input type=\"radio\" name=\"objtype\" value=\"quicktimemovie\">Quicktime Movie<br>");
    printf( "   </td></tr>\n");

    printf( "   <tr><td>This document is a translation of this object<br>(enter object reference number):</td>\n");
    printf( "   <td><input name=\"translates\" type=\"text\">\n");
    printf( "   </td></tr>\n");

    printf( "   <tr><td>This document is a new version of this previous object<br>(enter object reference number):</td>\n");
    printf( "   <td><input name=\"updates\" type=\"text\">\n");
    printf( "   </td></tr>\n");

    printf( "   <tr><td>This document is a new fork of this previous object<br>(enter object reference number):</td>\n");
    printf( "   <td><input name=\"forkof\" type=\"text\">\n");
    printf( "   </td></tr>\n");

    printf( "</table>\n");
 
    printf ("<br>");
    printf ("<input type=\"submit\" name=\"submit\" value=\"Search\">\n");
    printf ("<input type=\"hidden\" name=\"command\" value=\"search\">\n");
    printf( "</form>\n" );


    printf ("<form action=\"dart\">\n");
    printf ("<input type=\"hidden\" name=\"username\" value=\"%s\">\n",fieldvalue[username_param]);
    printf ("<input type=\"hidden\" name=\"password\" value=\"%s\">\n",fieldvalue[password_param]);
    printf ("<input type=\"submit\" name=\"submit\" value=\"Back to menu\">\n");
    printf ("<input type=\"hidden\" name=\"command\" value=\"menu\">\n");
    printf ("</form><br>\n" );

    printf ("   </body>\n");
    printf ("</html>\n");
    return 0;
  };

  if (strcmp(&fieldvalue[command_param][0],"search")==0) {
    //search results are displayed

    shresult=system ("/usr/bin/mysql -B -H  -e \"USE jeff; SELECT * FROM mytable;\" > x.tmp");   //add -H to get HTML output
    infile=fopen("x.tmp","r");
    while (feof(infile)==0) {
        x=fgetc(infile);
        if (x!='ÿ') {
           putchar(x);
        };

    };
    fclose(infile);        

    printf ("Logged in as: %s<br><br>",fieldvalue[username_param]);
    printf ("<input type=\"hidden\" name=\"username\" value=\"%s\">\n",fieldvalue[username_param]);
    printf ("<input type=\"hidden\" name=\"password\" value=\"%s\">\n",fieldvalue[password_param]);
    printf( "<h2>Search Results Page</h2><br>");
    printf ("   </body>\n");

    printf ("<form action=\"dart\">\n");
    printf ("<input type=\"hidden\" name=\"username\" value=\"%s\">\n",fieldvalue[username_param]);
    printf ("<input type=\"hidden\" name=\"password\" value=\"%s\">\n",fieldvalue[password_param]);
    printf ("<input type=\"submit\" name=\"submit\" value=\"Back to menu\">\n");
    printf ("<input type=\"hidden\" name=\"command\" value=\"menu\">\n");
    printf ("</form><br>\n" );

    printf ("</html>\n");
    return 0;
  };

  if (strcmp(&fieldvalue[command_param][0],"readdocumentmeta")==0) {
    //search results are displayed
    printf ("Logged in as: %s<br><br>",fieldvalue[username_param]);
    printf ("<input type=\"hidden\" name=\"username\" value=\"%s\">\n",fieldvalue[username_param]);
    printf ("<input type=\"hidden\" name=\"password\" value=\"%s\">\n",fieldvalue[password_param]);
    printf( "<h2>Document Metadata (Read Only)</h2><br>");
    printf ("   </body>\n");

    printf ("<form action=\"dart\">\n");
    printf ("<input type=\"hidden\" name=\"username\" value=\"%s\">\n",fieldvalue[username_param]);
    printf ("<input type=\"hidden\" name=\"password\" value=\"%s\">\n",fieldvalue[password_param]);
    printf ("<input type=\"submit\" name=\"submit\" value=\"Back to menu\">\n");
    printf ("<input type=\"hidden\" name=\"command\" value=\"menu\">\n");
    printf ("</form><br>\n" );

    printf ("</html>\n");
    return 0;
  };

  printf ("System Error - No command was specified. Please contact your system administrator for more\n");
  printf ("information on this fault. Your system might not be configured correctly, or there might be\n");
  printf ("a bug in the system\n<br>");
  printf ("Command passed was <%s> \n<br>",&fieldvalue[command_param][0]);
  printf ("   </body>\n");
  printf ("</html>\n");
  return 0;

}



