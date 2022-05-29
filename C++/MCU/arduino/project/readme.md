# 工程项目

[参考1](https://github.com/HacoK/Arduino)

[NeoPixel 灯带控制库](https://github.com/adafruit/Adafruit_NeoPixel)

[DMX-shields  DMX512灯控制器 扩展版](https://playground.arduino.cc/DMX/DMXShield/)

[DMX-shields  DMX512灯控制器 扩展版2](https://item.taobao.com/item.htm?spm=a230r.1.14.16.6bf716667MCx4Z&id=585560386838&ns=1&abbucket=7#detail)

[DMX-shields  DMX512灯控制器 扩展版3](http://www.cqrobot.wiki/index.php/DMX_Shield_for_Arduino-CQR0260)

[DMX 控制库 DMXSerial ](https://github.com/mathertel/DMXSerial)

[DMX 控制库 DmxLightSource](https://github.com/mahaarbo/DmxLightSource)

[DMX 控制库 DMXSerial](https://github.com/Ewenwan/DMXSerial)

[DMX 控制库  Conceptinetics ](https://github.com/johnseghersmsft/Conceptinetics_RDM)

> Conceptinetics
```c

#include <Conceptinetics.h>

DMX_Master        dmx_master (100,2);

void setup()
{
    dmx_master.enable ();
}

void loop()
{
    dmx_master.setChannelValue(11,100);
    dmx_master.setChannelValue(14,100);
    delay(100);
}

```

> DmxMaster 
```c
#include <DmxMaster.h>

void setup()
{
    DmxMaster.usePin(4);
    DmxMaster.maxChannel(32);
}

void loop()
{
    DmxMaster.write(11, 100);
    DmxMaster.write(14, 100);
    delay(100);
}

```

## 灯光控制

调光技术科普:可控硅/0-10v/DALi/DMX 区别有哪些?哪个更好?

随着智能家居的理念逐渐走入大家的装修考虑范围,很多人对调光很感兴趣.那么,能实现调光的都有哪些技术分类呢?这些技术有哪些优劣呢?    大致上有4类:

    可控硅/前沿&后沿切向调光(模拟)
    1-10v/0-10v(模拟)
    DALi/DALi2(数字)
    DMX512(数字)
    

从市面上的调光技术/布线方法/造价 以及 未来实用性 来分别对这些做一个比较.

> 1. 可控硅/前沿&后沿切向调光

    一句话概括原理:通过直接对220V交流电的改造切掉一部分电流波,从而实现控制
   
优势:

    布线方法最简单
    不需要调光驱动,只需要模块
    同样适用于220v交流吊扇或者风机盘管速度/功率的控制

劣势:

    调光模块个头较大,对电箱空间和散热要求最高,灯光回路越多越占空间
    布线费用高
    不适用于现在主流的无主灯照明设计(现在大部分光源是LED 低压)

> 2. 1-10v/0-10v

    一句话概括原理:模拟电压信号控制(例如1v对应亮度是0%,5v对应40%,10v对应100%,绝大部分不是线性的曲线)

优点:

    调光驱动价格低
    布线原理简单

缺点:

    布线费用最高
    调光模块个头较大,对电箱空间要求略高
    线路长的话会有压降(例如10v的信号在接收端降到了9.5v,调光会略有失真)
    在灯光回路数不多的情况下 比DALi成本低(例如只有一两个房间)
    
> 2. DALi/DALi2

    一句话概括原理:每个灯具都是一个单独的点,可以自由组合成回路
    
    
优点:

    照明控制行业里的标杆,升级空间大,一次布线至少可以使用三十年,现在已经有DALi2,对双色温和RGB有了更好的支持,而且不需要对线路进行改动
    数字信号,可以分组/控制自由,既可以对组进行控制,也可以对单个灯具进行控制
    可以定义灯开启的默认亮度/灯渐凉渐灭的时间长度/跑马灯/多个回路按照顺序亮起(比如玄关灯立刻渐渐亮起 5秒后客厅灯带渐渐亮起,再过5秒玄关灯慢慢灭掉)
    调光模块个头最小,对电箱空间要求非常低(约四五个房间共占用2个空气开关的位置)
    布线原理简单且费用折中,体量越大越划算(例如超过三四个房间)
    抗干扰能力最强(因为速率最低嘛,又不需要DMX那样 一秒钟内512个通道内变化好几十次)

缺点:

    相对于DMX码率低一些(但是DALi稳如狗啊)
    驱动价格相对其他方案略高一些
    驱动价格高

DMX512


    百度百科链接:https://baike.baidu.com/item/DMX512/2904121?fromtitle=DMX%20512
    
    一句话概括原理:控制器在不断控制512个通道的值,每个灯具只要监听某通道的值就知道自己要做什么
    DMX要单独举一些灯具的例子说一下才方便理解
    
    例子1:某单个调光占1个通道:0-255值  0 = 亮度0%,255= 亮度100%
    例子2:某RGB灯带站3个通道:255/0/0表示红色亮度100%,绿和蓝全灭,最后就是得到红,同理255/255/255表示红绿蓝全100%,最后得到白光全亮,这时候灯具功率最高
    例子3:某品牌摇头图案灯占14个通道
    通道1:灯头水平角度X轴
    通道2:灯头垂直角度Y轴
    通道3:dimming
    通道4:频闪 0~10 长灭/11~250 从先亮10秒再灭10秒~每秒频闪10次(闪瞎狗眼的那种效果)/251~255 全亮 不频闪
    通道5/6/7/8:RGBW 用于渲染氛围颜色 红绿蓝和白光
    通道9:图案形状:0~10 无图案/11~50 五角星/51~100圆圈/101~150花朵/151~200小动物/201~255 方块
    通道10:图案的漂移速度:0~10 固定/11-255 逐渐变快
    通道11/12/13/14:图案的颜色:RGBW

优点:

    数字信号,布线方式和DALi类似
    最炫酷的灯光肯定出自DMX

缺点:

    需要精心编排,适合于舞台灯光,不适合家用(影音室除外)


总结

    在未来LED肯定是照明灯具的主流,因为更节能环保,而且有效减少炫光,还容易附加渐亮/渐灭的效果.
    所以我认为,数字信号+驱动的方式是未来调光的技术主流(例如北京大兴机场就是全部DALi).而其中的DALi是最适合家用的照明控制系统,DMX仅仅适用于做影音室里伴随K歌的炫酷吊炸天效果.
