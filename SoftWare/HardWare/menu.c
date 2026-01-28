#include "zf_common_headfile.h"
#include "zf_device_tft180.h"
#include "zf_device_key.h"


void showpalce0(uint8 x){
	switch(x){
		case 1:
			tft180_show_string(0,10, "    MAIN MANU    ");
			tft180_show_string(0,30, "----------------");
		    tft180_show_string(0,50," >MODE1: balance");
			tft180_show_string(0,60,"  MODE2: run circle");
		    tft180_show_string(0,70,"  MODE3: run 8");
			tft180_show_string(0,80,"  MODE4: path memory");
		    tft180_show_string(0,90,"  MODE5: remote control");
			tft180_show_string(0,100,"  PID setting");
			break;
		case 2:
			tft180_show_string(0,10, "    MAIN MANU    ");
			tft180_show_string(0,30, "----------------");
		    tft180_show_string(0,50,"  MODE1: balance");
			tft180_show_string(0,60," >MODE2: run circle");
		    tft180_show_string(0,70,"  MODE3: run 8");
			tft180_show_string(0,80,"  MODE4: path memory");
		    tft180_show_string(0,90,"  MODE5: remote control");
			tft180_show_string(0,100,"  PID setting");
			break;
		case 3:
			tft180_show_string(0,10, "    MAIN MANU    ");
			tft180_show_string(0,30, "----------------");
		    tft180_show_string(0,50,"  MODE1: balance");
			tft180_show_string(0,60,"  MODE2: run circle");
		    tft180_show_string(0,70," >MODE3: run 8");
			tft180_show_string(0,80,"  MODE4: path memory");
		    tft180_show_string(0,90,"  MODE5: remote control");
			tft180_show_string(0,100,"  PID setting");
			break;
		case 4:
			tft180_show_string(0,10, "    MAIN MANU    ");
			tft180_show_string(0,30, "----------------");
		    tft180_show_string(0,50,"  MODE1: balance");
			tft180_show_string(0,60,"  MODE2: run circle");
		    tft180_show_string(0,70,"  MODE3: run 8");
			tft180_show_string(0,80," >MODE4: path memory");
		    tft180_show_string(0,90,"  MODE5: remote control");
			tft180_show_string(0,100,"  PID setting");
			break;
		case 5:
			tft180_show_string(0,10, "    MAIN MANU    ");
			tft180_show_string(0,30, "----------------");
		    tft180_show_string(0,50,"  MODE1: balance");
			tft180_show_string(0,60,"  MODE2: run circle");
		    tft180_show_string(0,70,"  MODE3: run 8");
			tft180_show_string(0,80,"  MODE4: path memory");
		    tft180_show_string(0,90," >MODE5: remote control");
			tft180_show_string(0,100,"  PID setting");
			break;
		case 6:
			tft180_show_string(0,10, "    MAIN MANU    ");
			tft180_show_string(0,30, "----------------");
		    tft180_show_string(0,50,"  MODE1: balance");
			tft180_show_string(0,60,"  MODE2: run circle");
			tft180_show_string(0,70,"  MODE3: run 8");
			tft180_show_string(0,80,"  MODE4: path memory");
		    tft180_show_string(0,90,"  MODE5: remote control");
			tft180_show_string(0,100," >PID setting");
			break;
	}
}

void showplace1(uint8 x){

	switch(x){
		case 1:
			tft180_show_string(0, 10, "    MODE 1 balance  ");
			tft180_show_string(0, 50, "    >READY");
			tft180_show_string(0,60 ,"              ");
			tft180_show_string(0,70 ,"              ");
			tft180_show_string(0,80 ,"              ");
			tft180_show_string(0,90 ,"              ");
			tft180_show_string(0,100,"              ");
			tft180_show_string(0,110,"              ");
			tft180_show_string(0,120,"              ");
		    break;
		case 2:
			tft180_show_string(0, 10, "    MODE 1 balance");
			tft180_show_string(0, 50, "    GO!GO!GO!");
			tft180_show_string(0,60 ,"              ");
			tft180_show_string(0,70 ,"              ");
			tft180_show_string(0,80 ,"              ");
			tft180_show_string(0,90 ,"              ");
			tft180_show_string(0,100,"              ");
			tft180_show_string(0,110,"              ");
			tft180_show_string(0,120,"              ");
			break;
	}
}

void showplace2(uint8 x){
	switch(x){
		case 1:
			tft180_show_string(0, 10, "    MODE 2: run circle");
			tft180_show_string(0, 50, "    >READY");
			tft180_show_string(0,60 ,"              ");
			tft180_show_string(0,70 ,"              ");
			tft180_show_string(0,80 ,"              ");
			tft180_show_string(0,90 ,"              ");
			tft180_show_string(0,100,"              ");
			tft180_show_string(0,110,"              ");
			tft180_show_string(0,120,"              ");
		    break;
		case 2:
			tft180_show_string(0, 10, "    MODE 2: run circle");
			tft180_show_string(0, 50, "   GO!GO!GO! ");
			tft180_show_string(0,60 ,"              ");
			tft180_show_string(0,70 ,"              ");
			tft180_show_string(0,80 ,"              ");
			tft180_show_string(0,90 ,"              ");
			tft180_show_string(0,100,"              ");
			tft180_show_string(0,110,"              ");
			tft180_show_string(0,120,"              ");
			break;
	}
}

void showplace3(uint8 x){
	switch(x){
		case 1:
			tft180_show_string(0, 10, "    MODE 3: run 8");
			tft180_show_string(0, 50, "   >READY");
			tft180_show_string(0,60 ,"              ");
			tft180_show_string(0,70 ,"              ");
			tft180_show_string(0,80 ,"              ");
			tft180_show_string(0,90 ,"              ");
			tft180_show_string(0,100,"              ");
			tft180_show_string(0,110,"              ");
			tft180_show_string(0,120,"              ");
		    break;
		case 2:
			tft180_show_string(0, 10, "    MODE 3: run 8");
			tft180_show_string(0, 50, "   GO!GO!GO! ");
			tft180_show_string(0,60 ,"              ");
			tft180_show_string(0,70 ,"              ");
			tft180_show_string(0,80 ,"              ");
			tft180_show_string(0,90 ,"              ");
			tft180_show_string(0,100,"              ");
			tft180_show_string(0,110,"              ");
			tft180_show_string(0,120,"              ");
			break;
	}
}

void showplace4(uint8 x){
	switch(x){
		case 1:
			tft180_show_string(0, 10, "    MODE 4: path memory");
			tft180_show_string(0, 50, "  >READY");
			tft180_show_string(0,60 ,"              ");
			tft180_show_string(0,70 ,"              ");
			tft180_show_string(0,80 ,"              ");
			tft180_show_string(0,90 ,"              ");
			tft180_show_string(0,100,"              ");
			tft180_show_string(0,110,"              ");
			tft180_show_string(0,120,"              ");
		    break;
		case 2:
			tft180_show_string(0, 10, "    MODE 4: path memory");
			tft180_show_string(0, 50, "   GO!GO!GO! ");
			tft180_show_string(0,60 ,"              ");
			tft180_show_string(0,70 ,"              ");
			tft180_show_string(0,80 ,"              ");
			tft180_show_string(0,90 ,"              ");
			tft180_show_string(0,100,"              ");
			tft180_show_string(0,110,"              ");
			tft180_show_string(0,120,"              ");
			break;
	}
}

void showplace5(uint8 x){
	switch(x){
		case 1:
			tft180_show_string(0, 10, "    MODE 5: remote control");
			tft180_show_string(0, 50, "   >READY");
			tft180_show_string(0,60 ,"              ");
			tft180_show_string(0,70 ,"              ");
			tft180_show_string(0,80 ,"              ");
			tft180_show_string(0,90 ,"              ");
			tft180_show_string(0,100,"              ");
			tft180_show_string(0,110,"              ");
			tft180_show_string(0,120,"              ");
		    break;
		case 2:
			tft180_show_string(0, 10, "    MODE 5: remote control");
			tft180_show_string(0, 50, "   GO!GO!GO! ");
			tft180_show_string(0,60 ,"              ");
			tft180_show_string(0,70 ,"              ");
			tft180_show_string(0,80 ,"              ");
			tft180_show_string(0,90 ,"              ");
			tft180_show_string(0,100,"              ");
			tft180_show_string(0,110,"              ");
			tft180_show_string(0,120,"              ");
			break;
	}
}

void showplace6(uint8 x,uint8 che,float kp,float ki,float kd){
	if(che!=0)tft180_show_string(64,10,"+");
	else if(che==0)tft180_show_string(64,10,"-");
	switch(x){
		case 1:
			tft180_show_string(0, 10, "    PID setting");
			tft180_show_string( 0, 50, "  >KP:");
			tft180_show_string( 0, 70, "   KI:");
			tft180_show_string( 0, 90, "   KD:");
			tft180_show_float(40,50,kp,1,1);
			tft180_show_float(40,70,ki,1,1);
			tft180_show_float(40,90,kd,1,1);
			tft180_show_string(0,60 ,"              ");
			tft180_show_string(0,80 ,"              ");
			tft180_show_string(0,100,"              ");
			tft180_show_string(0,110,"              ");
			tft180_show_string(0,120,"              ");
			break;
		case 2:
			tft180_show_string(0, 10, "    PID setting");
			tft180_show_string( 0, 50, "   KP:");
			tft180_show_string( 0, 70, "  >KI:");
			tft180_show_string( 0, 90, "   KD:");
			tft180_show_float(40,50,kp,1,1);
			tft180_show_float(40,70,ki,1,1);
			tft180_show_float(40,90,kd,1,1);
			tft180_show_string(0,60 ,"              ");
			tft180_show_string(0,80 ,"              ");
			tft180_show_string(0,100,"              ");
			tft180_show_string(0,110,"              ");
			tft180_show_string(0,120,"              ");
			break;
		case 3:
			tft180_show_string(0, 10, "    PID setting");
			tft180_show_string( 0, 50, "   KP:");
			tft180_show_string( 0, 70, "   KI:");
			tft180_show_string( 0, 90, "  >KD:");
			tft180_show_float(40,50,kp,1,1);
			tft180_show_float(40,70,ki,1,1);
			tft180_show_float(40,90,kd,1,1);
			tft180_show_string(0,60 ,"              ");
			tft180_show_string(0,80 ,"              ");
			tft180_show_string(0,100,"              ");
			tft180_show_string(0,110,"              ");
			tft180_show_string(0,120,"              ");
			break;
	}
}

uint8 che=0;

void menu(uint8* xp,uint8* yp,float* kp,float* ki,float* kd){
	tft180_set_font (0);
	if(*yp==0){
			
		showpalce0(*xp);
		if(key_get_state(KEY_1)==KEY_SHORT_PRESS){
			*xp+=1;
			key_clear_state(KEY_1);
		}
		if(key_get_state(KEY_2)==KEY_SHORT_PRESS){
			*xp-=1;
			key_clear_state(KEY_2);
		}
		if(key_get_state(KEY_3)==KEY_SHORT_PRESS){
			*yp+=1;
			key_clear_state(KEY_3);
		}
		if(*xp>6)*xp=1;
		if(*xp<1)*xp=6;
	}
	else if(*yp!=0){
		if(*xp==1){
			showplace1(*yp);
			if(key_get_state(KEY_3)==KEY_SHORT_PRESS){
				*yp+=1;
				key_clear_state(KEY_3);
			}
			if(*yp>2)*yp=1;
			if(key_get_state(KEY_4)==KEY_SHORT_PRESS){
				*yp=0;
				key_clear_state(KEY_4);
			}			
		}
		else if(*xp==2){
			showplace2(*yp);
			if(key_get_state(KEY_3)==KEY_SHORT_PRESS){
				*yp+=1;
				key_clear_state(KEY_3);
			}
			if(*yp>2)*yp=1;
			if(key_get_state(KEY_4)==KEY_SHORT_PRESS){
				*yp=0;
				key_clear_state(KEY_4);
			}			
		}
		else if(*xp==3){
			showplace3(*yp);
			if(key_get_state(KEY_3)==KEY_SHORT_PRESS){
				*yp+=1;
				key_clear_state(KEY_3);
			}
			if(*yp>2)*yp=1;
			if(key_get_state(KEY_4)==KEY_SHORT_PRESS){
				*yp=0;
				key_clear_state(KEY_4);
			}	
		}
		else if(*xp==4){
			showplace4(*yp);
			if(key_get_state(KEY_3)==KEY_SHORT_PRESS){
				*yp+=1;
				key_clear_state(KEY_3);
			}
			if(*yp>2)*yp=1;
			if(key_get_state(KEY_4)==KEY_SHORT_PRESS){
				*yp=0;
				key_clear_state(KEY_4);
			}	
		}
		else if(*xp==5){
			showplace5(*yp);
			if(key_get_state(KEY_3)==KEY_SHORT_PRESS){
				*yp+=1;
				key_clear_state(KEY_3);
			}
			if(*yp>2)*yp=1;
			if(key_get_state(KEY_4)==KEY_SHORT_PRESS){
				*yp=0;
				key_clear_state(KEY_4);
			}
		}
		else if(*xp==6){
			if(che==0){
				if(key_get_state(KEY_1)==KEY_SHORT_PRESS){
					*yp+=1;
					key_clear_state(KEY_1);
				}
				if(key_get_state(KEY_2)==KEY_SHORT_PRESS){
					*yp-=1;
					key_clear_state(KEY_2);
				}
				if(*yp>3)*yp=1;
				if(*yp<1)*yp=3;
				if(key_get_state(KEY_4)==KEY_SHORT_PRESS){
					*yp=0;
					che=0;
					key_clear_state(KEY_4);
				}
				if(key_get_state(KEY_3)==KEY_SHORT_PRESS){
					che=!che;
					key_clear_state(KEY_3);
				}
			}
			else if(che!=0){
				if(*yp==1){
					if(key_get_state(KEY_1)==KEY_SHORT_PRESS){
						*kp+=0.1;
						key_clear_state(KEY_1);
					}
					if(key_get_state(KEY_2)==KEY_SHORT_PRESS){
						*kp-=0.1;
						key_clear_state(KEY_2);
					}
					if(key_get_state(KEY_1)==KEY_LONG_PRESS){
						*kp+=0.5;
						key_clear_state(KEY_1);
					}
					if(key_get_state(KEY_2)==KEY_LONG_PRESS){
						*kp-=0.5;
						key_clear_state(KEY_2);
					}
				}
				else if(*yp==2){
					if(key_get_state(KEY_1)==KEY_SHORT_PRESS){
						*ki+=0.1;
						key_clear_state(KEY_1);
					}
					if(key_get_state(KEY_2)==KEY_SHORT_PRESS){
						*ki-=0.1;
						key_clear_state(KEY_2);
					}
					if(key_get_state(KEY_1)==KEY_LONG_PRESS){
						*ki+=0.5;
						key_clear_state(KEY_1);
					}
					if(key_get_state(KEY_2)==KEY_LONG_PRESS){
						*ki-=0.5;
						key_clear_state(KEY_2);
					}
				}
				else if(*yp==3){
					if(key_get_state(KEY_1)==KEY_SHORT_PRESS){
						*kd+=0.1;
						key_clear_state(KEY_1);
					}
					if(key_get_state(KEY_2)==KEY_SHORT_PRESS){
						*kd-=0.1;
						key_clear_state(KEY_2);
					}
					if(key_get_state(KEY_1)==KEY_LONG_PRESS){
						*kd+=0.5;
						key_clear_state(KEY_1);
					}
					if(key_get_state(KEY_2)==KEY_LONG_PRESS){
						*kd-=0.5;
						key_clear_state(KEY_2);
					}
				}
				if(key_get_state(KEY_3)==KEY_SHORT_PRESS){
					che=!che;
					key_clear_state(KEY_3);
				}
			}
			showplace6(*yp,che,*kp,*ki,*kd);
		}
	}
}
