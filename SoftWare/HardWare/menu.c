#include "zf_common_headfile.h"
#include "zf_device_tft180.h"
#include "zf_device_key.h"


void showpalce0(uint8 x){
	tft180_clear();
	switch(x){
		case 0:
			tft180_show_string(10, 5, "总菜单");
			tft180_show_string(30,16,">模式1：原地不动");
			tft180_show_string(50,16," 模式2：跑道巡线");
			tft180_show_string(70,16," 模式3：8字巡线");
			tft180_show_string(90,16," 模式4：记忆路径");
			tft180_show_string(110,16," 模式5：蓝牙操作");
			tft180_show_string(130,16," 参数调整");
			break;
		case 1:
			tft180_show_string(10, 5, "总菜单");
			tft180_show_string(30,16," 模式1：原地不动");
			tft180_show_string(50,16,">模式2：跑道巡线");
			tft180_show_string(70,16," 模式3：8字巡线");
			tft180_show_string(90,16," 模式4：记忆路径");
			tft180_show_string(110,16," 模式5：蓝牙操作");
			tft180_show_string(130,16," 参数调整");
			break;
		case 2:
			tft180_show_string(10, 5, "总菜单");
			tft180_show_string(30,16," 模式1：原地不动");
			tft180_show_string(50,16," 模式2：跑道巡线");
			tft180_show_string(70,16,">模式3：8字巡线");
			tft180_show_string(90,16," 模式4：记忆路径");
			tft180_show_string(110,16," 模式5：蓝牙操作");
			tft180_show_string(130,16," 参数调整");
			break;
		case 3:
			tft180_show_string(10, 5, "总菜单");
			tft180_show_string(30,16," 模式1：原地不动");
			tft180_show_string(50,16," 模式2：跑道巡线");
			tft180_show_string(70,16," 模式3：8字巡线");
			tft180_show_string(90,16,">模式4：记忆路径");
			tft180_show_string(110,16," 模式5：蓝牙操作");
			tft180_show_string(130,16," 参数调整");
			break;
		case 4:
			tft180_show_string(10, 5, "总菜单");
			tft180_show_string(30,16," 模式1：原地不动");
			tft180_show_string(50,16," 模式2：跑道巡线");
			tft180_show_string(70,16," 模式3：8字巡线");
			tft180_show_string(90,16," 模式4：记忆路径");
			tft180_show_string(110,16,">模式5：蓝牙操作");
			tft180_show_string(130,16," 参数调整");
			break;
		case 5:
			tft180_show_string(10, 5, "总菜单");
			tft180_show_string(30,16," 模式1：原地不动");
			tft180_show_string(50,16," 模式2：跑道巡线");
			tft180_show_string(70,16," 模式3：8字巡线");
			tft180_show_string(90,16," 模式4：记忆路径");
			tft180_show_string(110,16," 模式5：蓝牙操作");
			tft180_show_string(130,16,">参数调整");
			break;
	}
}

void showplace1(uint8 x){
	tft180_clear();
	switch(x){
		case 1:
			tft180_show_string(10, 5, "模式1：原地不动");
			tft180_show_string(30, 16, ">发车");
		    break;
		case 2:
			tft180_show_string(10, 5, "模式1：原地不动");
			tft180_show_string(30, 16, "冲刺，冲！");
			break;
	}
}

void showplace2(uint8 x){
	tft180_clear();
	switch(x){
		case 1:
			tft180_show_string(10, 5, "模式2：跑道巡线");
			tft180_show_string(30, 16, ">发车");
		    break;
		case 2:
			tft180_show_string(10, 5, "模式2：跑道巡线");
			tft180_show_string(30, 16, "冲刺，冲！");
			break;
	}
}

void showplace3(uint8 x){
	tft180_clear();
	switch(x){
		case 1:
			tft180_show_string(10, 5, "模式3：8字巡线");
			tft180_show_string(30, 16, ">发车");
		    break;
		case 2:
			tft180_show_string(10, 5, "模式3：8字巡线");
			tft180_show_string(30, 16, "冲刺，冲！");
			break;
	}
}

void showplace4(uint8 x){
	tft180_clear();
	switch(x){
		case 1:
			tft180_show_string(10, 5, "模式4：记忆路径");
			tft180_show_string(30, 16, ">发车");
		    break;
		case 2:
			tft180_show_string(10, 5, "模式4：记忆路径");
			tft180_show_string(30, 16, "冲刺，冲！");
			break;
	}
}

void showplace5(uint8 x){
	tft180_clear();
	switch(x){
		case 1:
			tft180_show_string(10, 5, "模式5：蓝牙操作");
			tft180_show_string(30, 16, ">发车");
		    break;
		case 2:
			tft180_show_string(10, 5, "模式5：蓝牙操作");
			tft180_show_string(30, 16, "冲刺，冲！");
			break;
	}
}

void showplace6(uint8 x,uint8 che,float kp,float ki,float kd){
	tft180_clear();
	if(che!=0)tft180_show_string(10,64,"*");
	switch(x){
		case 1:
			tft180_show_string(10, 5, "参数调整");
			tft180_show_string(30, 16, ">KP:");
			tft180_show_string(50, 16, " KI:");
			tft180_show_string(70, 16, " KD:");
			tft180_show_float(30,32,kp,1,1);
			tft180_show_float(50,32,ki,1,1);
			tft180_show_float(70,32,kd,1,1);
			break;
		case 2:
			tft180_show_string(10, 5, "参数调整");
			tft180_show_string(30, 16, " KP:");
			tft180_show_string(50, 16, ">KI:");
			tft180_show_string(70, 16, " KD:");
			tft180_show_float(30,32,kp,1,1);
			tft180_show_float(50,32,ki,1,1);
			tft180_show_float(70,32,kd,1,1);
			break;
		case 3:
			tft180_show_string(10, 5, "参数调整");
			tft180_show_string(30, 16, " KP:");
			tft180_show_string(50, 16, " KI:");
			tft180_show_string(70, 16, ">KD:");
			tft180_show_float(30,32,kp,1,1);
			tft180_show_float(50,32,ki,1,1);
			tft180_show_float(70,32,kd,1,1);
			break;
	}
}

uint8 che=0;

void menu(uint8 xp,uint8 yp,float* kp,float* ki,float* kd){
	if(yp==0){
		showpalce0(xp);
		if(key_get_state(KEY_1)==KEY_SHORT_PRESS){
			xp++;
			key_clear_state(KEY_1);
		}
		if(key_get_state(KEY_2)==KEY_SHORT_PRESS){
			xp--;
			key_clear_state(KEY_2);
		}
		if(key_get_state(KEY_3)==KEY_SHORT_PRESS){
			yp++;
			key_clear_state(KEY_3);
		}
		if(xp>5)xp=0;
		if(xp<0)xp=5;
	}
	else if(yp==1){
		if(xp==0){
			showplace1(yp);
			if(key_get_state(KEY_3)==KEY_SHORT_PRESS){
				yp++;
				key_clear_state(KEY_3);
			}
			if(yp>2)yp=1;
			if(key_get_state(KEY_4)==KEY_SHORT_PRESS){
				yp=0;
				key_clear_state(KEY_4);
			}			
		}
		else if(xp==1){
			showplace2(yp);
			if(key_get_state(KEY_3)==KEY_SHORT_PRESS){
				yp++;
				key_clear_state(KEY_3);
			}
			if(yp>2)yp=1;
			if(key_get_state(KEY_4)==KEY_SHORT_PRESS){
				yp=0;
				key_clear_state(KEY_4);
			}			
		}
		else if(xp==2){
			showplace3(yp);
			if(key_get_state(KEY_3)==KEY_SHORT_PRESS){
				yp++;
				key_clear_state(KEY_3);
			}
			if(yp>2)yp=1;
			if(key_get_state(KEY_4)==KEY_SHORT_PRESS){
				yp=0;
				key_clear_state(KEY_4);
			}	
		}
		else if(xp==3){
			showplace4(yp);
			if(key_get_state(KEY_3)==KEY_SHORT_PRESS){
				yp++;
				key_clear_state(KEY_3);
			}
			if(yp>2)yp=1;
			if(key_get_state(KEY_4)==KEY_SHORT_PRESS){
				yp=0;
				key_clear_state(KEY_4);
			}	
		}
		else if(xp==4){
			showplace5(yp);
			if(key_get_state(KEY_3)==KEY_SHORT_PRESS){
				yp++;
				key_clear_state(KEY_3);
			}
			if(yp>2)yp=1;
			if(key_get_state(KEY_4)==KEY_SHORT_PRESS){
				yp=0;
				key_clear_state(KEY_4);
			}
		}
		else if(xp==5){
			if(che==0){
				if(key_get_state(KEY_1)==KEY_SHORT_PRESS){
					yp++;
					key_clear_state(KEY_1);
				}
				if(key_get_state(KEY_2)==KEY_SHORT_PRESS){
					yp--;
					key_clear_state(KEY_2);
				}
				if(yp>3)yp=1;
				if(yp<1)yp=3;
				if(key_get_state(KEY_4)==KEY_SHORT_PRESS){
					yp=0;
					che=0;
					key_clear_state(KEY_4);
				}
				if(key_get_state(KEY_3)==KEY_SHORT_PRESS){
					che=!che;
					key_clear_state(KEY_3);
				}
			}
			else if(che!=0){
				if(yp==1){
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
				else if(yp==2){
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
				else if(yp==3){
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
			showplace6(yp,che,*kp,*ki,*kd);
		}
	}
}
