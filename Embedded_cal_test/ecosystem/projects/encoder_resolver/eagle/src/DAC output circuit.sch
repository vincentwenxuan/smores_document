<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="6.5.0">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting verticaltext="up"/>
</settings>
<grid distance="50" unitdist="mil" unit="mil" style="lines" multiple="2" display="yes" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="2" name="Route2" color="1" fill="3" visible="no" active="no"/>
<layer number="3" name="Route3" color="4" fill="3" visible="no" active="no"/>
<layer number="14" name="Route14" color="1" fill="6" visible="no" active="no"/>
<layer number="15" name="Route15" color="4" fill="6" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
<layer number="99" name="SpiceOrder" color="5" fill="1" visible="no" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="lt-supply">
<description>Die SUPPLY-Symbole für LT-Spice&lt;p&gt;
0 für GND&lt;br&gt;
COM für Masse&lt;p&gt;
librarian@cadsoft.de</description>
<packages>
</packages>
<symbols>
<symbol name="0">
<pin name="0" x="0" y="0" visible="off" length="point" direction="sup"/>
<wire x1="-2.54" y1="0" x2="2.54" y2="0" width="0.1524" layer="94"/>
<wire x1="2.54" y1="0" x2="0" y2="-2.54" width="0.1524" layer="94"/>
<wire x1="0" y1="-2.54" x2="-2.54" y2="0" width="0.1524" layer="94"/>
<text x="0" y="0" size="0.4064" layer="99" align="center">SpiceOrder 1</text>
</symbol>
</symbols>
<devicesets>
<deviceset name="0" prefix="V0_">
<gates>
<gate name="G$1" symbol="0" x="0" y="0"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="sym">
<description>LT-Spice library generated with :&lt;br&gt;
import-ltspice.ulp&lt;br&gt;
from: E:/Programme/LTC/LTspiceIV/lib/sym/*.asy&lt;br&gt;
at 12.06.2012 10:18:08</description>
<packages>
<package name="L1812">
<description>&lt;b&gt;INDUCTOR&lt;/b&gt;&lt;p&gt;
chip</description>
<wire x1="-1.4732" y1="1.6002" x2="1.4732" y2="1.6002" width="0.1016" layer="51"/>
<wire x1="-1.4478" y1="-1.6002" x2="1.4732" y2="-1.6002" width="0.1016" layer="51"/>
<smd name="1" x="-1.95" y="0" dx="1.9" dy="3.4" layer="1"/>
<smd name="2" x="1.95" y="0" dx="1.9" dy="3.4" layer="1"/>
<text x="-1.905" y="1.905" size="1.27" layer="25">&gt;NAME</text>
<text x="-1.905" y="-3.175" size="1.27" layer="27">&gt;VALUE</text>
<rectangle x1="-2.3876" y1="-1.651" x2="-1.4376" y2="1.649" layer="51"/>
<rectangle x1="1.4478" y1="-1.651" x2="2.3978" y2="1.649" layer="51"/>
</package>
<package name="R0805">
<description>&lt;b&gt;RESISTOR&lt;/b&gt;&lt;p&gt;</description>
<wire x1="-0.41" y1="0.635" x2="0.41" y2="0.635" width="0.1524" layer="51"/>
<wire x1="-0.41" y1="-0.635" x2="0.41" y2="-0.635" width="0.1524" layer="51"/>
<wire x1="-1.973" y1="0.983" x2="1.973" y2="0.983" width="0.0508" layer="39"/>
<wire x1="1.973" y1="0.983" x2="1.973" y2="-0.983" width="0.0508" layer="39"/>
<wire x1="1.973" y1="-0.983" x2="-1.973" y2="-0.983" width="0.0508" layer="39"/>
<wire x1="-1.973" y1="-0.983" x2="-1.973" y2="0.983" width="0.0508" layer="39"/>
<smd name="1" x="-0.95" y="0" dx="1.3" dy="1.5" layer="1"/>
<smd name="2" x="0.95" y="0" dx="1.3" dy="1.5" layer="1"/>
<text x="-0.635" y="1.27" size="1.27" layer="25">&gt;NAME</text>
<text x="-0.635" y="-2.54" size="1.27" layer="27">&gt;VALUE</text>
<rectangle x1="0.4064" y1="-0.6985" x2="1.0564" y2="0.7015" layer="51"/>
<rectangle x1="-1.0668" y1="-0.6985" x2="-0.4168" y2="0.7015" layer="51"/>
<rectangle x1="-0.1999" y1="-0.5001" x2="0.1999" y2="0.5001" layer="35"/>
</package>
<package name="C0805">
<description>&lt;b&gt;CAPACITOR&lt;/b&gt;&lt;p&gt;</description>
<wire x1="-1.973" y1="0.983" x2="1.973" y2="0.983" width="0.0508" layer="39"/>
<wire x1="1.973" y1="-0.983" x2="-1.973" y2="-0.983" width="0.0508" layer="39"/>
<wire x1="-1.973" y1="-0.983" x2="-1.973" y2="0.983" width="0.0508" layer="39"/>
<wire x1="-0.381" y1="0.66" x2="0.381" y2="0.66" width="0.1016" layer="51"/>
<wire x1="-0.356" y1="-0.66" x2="0.381" y2="-0.66" width="0.1016" layer="51"/>
<wire x1="1.973" y1="0.983" x2="1.973" y2="-0.983" width="0.0508" layer="39"/>
<smd name="1" x="-0.95" y="0" dx="1.3" dy="1.5" layer="1"/>
<smd name="2" x="0.95" y="0" dx="1.3" dy="1.5" layer="1"/>
<text x="-1.27" y="1.27" size="1.27" layer="25">&gt;NAME</text>
<text x="-1.27" y="-2.54" size="1.27" layer="27">&gt;VALUE</text>
<rectangle x1="-1.0922" y1="-0.7239" x2="-0.3421" y2="0.7262" layer="51"/>
<rectangle x1="0.3556" y1="-0.7239" x2="1.1057" y2="0.7262" layer="51"/>
<rectangle x1="-0.1001" y1="-0.4001" x2="0.1001" y2="0.4001" layer="35"/>
</package>
</packages>
<symbols>
<symbol name="IND2">
<circle x="1.27" y="-13.335" radius="0.635" width="0.1524" layer="94"/>
<wire x1="0.74395" y1="-10.68605" x2="0.74395" y2="-7.09395" width="0.1524" layer="94" curve="270"/>
<wire x1="2.54" y1="-15.24" x2="0.74395" y2="-10.90395" width="0.1524" layer="94" curve="225"/>
<wire x1="0.74395" y1="-6.87605" x2="2.54" y2="-2.540003125" width="0.1524" layer="94" curve="225"/>
<text x="5.715" y="-6.35" size="1.778" layer="95" align="center-left">&gt;NAME</text>
<text x="5.715" y="-12.7" size="1.778" layer="96" align="center-left">&gt;VALUE</text>
<pin name="A" x="2.54" y="-2.54" visible="off" length="point" direction="pas"/>
<text x="2.54" y="-2.54" size="0.4064" layer="99" align="center">SpiceOrder 1</text>
<pin name="B" x="2.54" y="-15.24" visible="off" length="point" direction="pas"/>
<text x="2.54" y="-15.24" size="0.4064" layer="99" align="center">SpiceOrder 2</text>
</symbol>
<symbol name="RES">
<wire x1="2.54" y1="-13.97" x2="2.54" y2="-15.24" width="0.1524" layer="94"/>
<wire x1="0" y1="-12.7" x2="2.54" y2="-13.97" width="0.1524" layer="94"/>
<wire x1="5.08" y1="-10.16" x2="0" y2="-12.7" width="0.1524" layer="94"/>
<wire x1="0" y1="-7.62" x2="5.08" y2="-10.16" width="0.1524" layer="94"/>
<wire x1="5.08" y1="-5.08" x2="0" y2="-7.62" width="0.1524" layer="94"/>
<wire x1="2.54" y1="-2.54" x2="2.54" y2="-3.81" width="0.1524" layer="94"/>
<wire x1="2.54" y1="-3.81" x2="5.08" y2="-5.08" width="0.1524" layer="94"/>
<text x="5.715" y="-6.35" size="1.778" layer="95" align="center-left">&gt;NAME</text>
<text x="5.715" y="-12.065" size="1.778" layer="96" align="center-left">&gt;VALUE</text>
<pin name="A" x="2.54" y="-2.54" visible="off" length="point" direction="pas"/>
<text x="2.54" y="-2.54" size="0.4064" layer="99" align="center">SpiceOrder 1</text>
<pin name="B" x="2.54" y="-15.24" visible="off" length="point" direction="pas"/>
<text x="2.54" y="-15.24" size="0.4064" layer="99" align="center">SpiceOrder 2</text>
</symbol>
<symbol name="CAP">
<wire x1="2.54" y1="-5.715" x2="2.54" y2="-10.16" width="0.1524" layer="94"/>
<wire x1="2.54" y1="-4.445" x2="2.54" y2="0" width="0.1524" layer="94"/>
<wire x1="0" y1="-4.445" x2="5.08" y2="-4.445" width="0.1524" layer="94"/>
<wire x1="0" y1="-5.715" x2="5.08" y2="-5.715" width="0.1524" layer="94"/>
<text x="3.81" y="-1.27" size="1.778" layer="95" align="center-left">&gt;NAME</text>
<text x="3.81" y="-8.89" size="1.778" layer="96" align="center-left">&gt;VALUE</text>
<pin name="A" x="2.54" y="0" visible="off" length="point" direction="pas"/>
<text x="2.54" y="0" size="0.4064" layer="99" align="center">SpiceOrder 1</text>
<pin name="B" x="2.54" y="-10.16" visible="off" length="point" direction="pas"/>
<text x="2.54" y="-10.16" size="0.4064" layer="99" align="center">SpiceOrder 2</text>
</symbol>
</symbols>
<devicesets>
<deviceset name="IND2" prefix="L" uservalue="yes">
<description>Inductor</description>
<gates>
<gate name="G$1" symbol="IND2" x="0" y="0"/>
</gates>
<devices>
<device name="" package="L1812">
<connects>
<connect gate="G$1" pin="A" pad="1"/>
<connect gate="G$1" pin="B" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="RES" prefix="R" uservalue="yes">
<description>A resistor</description>
<gates>
<gate name="G$1" symbol="RES" x="0" y="0"/>
</gates>
<devices>
<device name="" package="R0805">
<connects>
<connect gate="G$1" pin="A" pad="1"/>
<connect gate="G$1" pin="B" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="CAP" prefix="C" uservalue="yes">
<description>Capacitor</description>
<gates>
<gate name="G$1" symbol="CAP" x="0" y="0"/>
</gates>
<devices>
<device name="" package="C0805">
<connects>
<connect gate="G$1" pin="A" pad="1"/>
<connect gate="G$1" pin="B" pad="2"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="lt-spice-simulation">
<description>Library for simulation symbols only. All devices has no package, but a ATTRIBUTE "_EXTERNAL_", please read documentation!&lt;p&gt;
LT-Spice library generated with :&lt;br&gt;
import-ltspice.ulp&lt;br&gt;
from: E:/Programme/LTC/lib/sym/*.asy&lt;br&gt;
at 18.06.2012 12:14:32&lt;br&gt;</description>
<packages>
</packages>
<symbols>
<symbol name="VOLTAGE">
<wire x1="-1.27" y1="-5.715" x2="1.27" y2="-5.715" width="0.1524" layer="94"/>
<wire x1="-1.27" y1="-12.065" x2="1.27" y2="-12.065" width="0.1524" layer="94"/>
<wire x1="0" y1="-4.445" x2="0" y2="-6.985" width="0.1524" layer="94"/>
<wire x1="0" y1="-15.24" x2="0" y2="-13.97" width="0.1524" layer="94"/>
<wire x1="0" y1="-2.54" x2="0" y2="-3.81" width="0.1524" layer="94"/>
<circle x="0" y="-8.89" radius="5.08" width="0.1524" layer="94"/>
<text x="3.81" y="-2.54" size="1.778" layer="95" align="center-left">&gt;NAME</text>
<text x="3.81" y="-15.24" size="1.778" layer="96" align="center-left">&gt;VALUE</text>
<pin name="+" x="0" y="-2.54" visible="off" length="point" direction="pas"/>
<text x="0" y="-2.54" size="0.4064" layer="99" align="center">SpiceOrder 1</text>
<pin name="-" x="0" y="-15.24" visible="off" length="point" direction="pas"/>
<text x="0" y="-15.24" size="0.4064" layer="99" align="center">SpiceOrder 2</text>
</symbol>
<symbol name="BV">
<wire x1="-1.27" y1="-5.715" x2="1.27" y2="-5.715" width="0.1524" layer="94"/>
<wire x1="-1.27" y1="-12.065" x2="1.27" y2="-12.065" width="0.1524" layer="94"/>
<wire x1="0" y1="-4.445" x2="0" y2="-6.985" width="0.1524" layer="94"/>
<wire x1="0" y1="-15.24" x2="0" y2="-13.97" width="0.1524" layer="94"/>
<wire x1="0" y1="-2.54" x2="0" y2="-3.81" width="0.1524" layer="94"/>
<circle x="0" y="-8.89" radius="5.08" width="0.1524" layer="94"/>
<text x="3.81" y="-2.54" size="1.778" layer="95" align="center-left">&gt;NAME</text>
<text x="3.81" y="-15.24" size="1.778" layer="96" align="center-left">&gt;VALUE</text>
<pin name="+" x="0" y="-2.54" visible="off" length="point" direction="pas"/>
<text x="0" y="-2.54" size="0.4064" layer="99" align="center">SpiceOrder 1</text>
<pin name="-" x="0" y="-15.24" visible="off" length="point" direction="pas"/>
<text x="0" y="-15.24" size="0.4064" layer="99" align="center">SpiceOrder 2</text>
</symbol>
</symbols>
<devicesets>
<deviceset name="VOLTAGE" prefix="V">
<description>Voltage Source, either DC, AC, PULSE, SINE, PWL, EXP, or SFFM</description>
<gates>
<gate name="G$1" symbol="VOLTAGE" x="0" y="0"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name="">
<attribute name="_EXTERNAL_" value="" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="BV" prefix="B">
<description>Arbitrary behavioral voltage source</description>
<gates>
<gate name="G$1" symbol="BV" x="0" y="0"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name="">
<attribute name="_EXTERNAL_" value="" constant="no"/>
</technology>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="V0_1" library="lt-supply" deviceset="0" device=""/>
<part name="V0_2" library="lt-supply" deviceset="0" device=""/>
<part name="V0_3" library="lt-supply" deviceset="0" device=""/>
<part name="V0_4" library="lt-supply" deviceset="0" device=""/>
<part name="V0_5" library="lt-supply" deviceset="0" device=""/>
<part name="V0_6" library="lt-supply" deviceset="0" device=""/>
<part name="V0_7" library="lt-supply" deviceset="0" device=""/>
<part name="V0_8" library="lt-supply" deviceset="0" device=""/>
<part name="V0_9" library="lt-supply" deviceset="0" device=""/>
<part name="V0_10" library="lt-supply" deviceset="0" device=""/>
<part name="V0_11" library="lt-supply" deviceset="0" device=""/>
<part name="V0_12" library="lt-supply" deviceset="0" device=""/>
<part name="V0_13" library="lt-supply" deviceset="0" device=""/>
<part name="L1" library="sym" deviceset="IND2" device="" value="9505">
<attribute name="SPICELINE" value="Rser=0.57"/>
<attribute name="SPICETYPE" value="ind"/>
</part>
<part name="L2" library="sym" deviceset="IND2" device="" value="237.55">
<attribute name="SPICELINE" value="Rser=0.35"/>
<attribute name="SPICETYPE" value="ind"/>
</part>
<part name="L3" library="sym" deviceset="IND2" device="" value="9505">
<attribute name="SPICELINE" value="Rser=0.57"/>
<attribute name="SPICETYPE" value="ind"/>
</part>
<part name="L4" library="sym" deviceset="IND2" device="" value="237.55">
<attribute name="SPICELINE" value="Rser=0.35"/>
<attribute name="SPICETYPE" value="ind"/>
</part>
<part name="R1" library="sym" deviceset="RES" device="" value="100GIG"/>
<part name="R2" library="sym" deviceset="RES" device="" value="1k"/>
<part name="R3" library="sym" deviceset="RES" device="" value="1k"/>
<part name="C1" library="sym" deviceset="CAP" device="" value="100n"/>
<part name="V2" library="lt-spice-simulation" deviceset="VOLTAGE" device="" value="3.3"/>
<part name="V3" library="lt-spice-simulation" deviceset="VOLTAGE" device="" value="5"/>
<part name="V4" library="lt-spice-simulation" deviceset="VOLTAGE" device="" value="SINE(0 1 20 0 0)"/>
<part name="V5" library="lt-spice-simulation" deviceset="VOLTAGE" device="" value="SINE(0 1.6 10k 0 0 90)"/>
<part name="B1" library="lt-spice-simulation" deviceset="BV" device="" value="V=V(vcarr) * V(vmod) +1.65"/>
<part name="R4" library="sym" deviceset="RES" device="" value="1m">
<attribute name="SPICELINE" value="Rser=1"/>
</part>
<part name="R5" library="sym" deviceset="RES" device="" value="1000GIG"/>
<part name="R6" library="sym" deviceset="RES" device="" value="500"/>
<part name="R7" library="sym" deviceset="RES" device="" value="15k"/>
<part name="C2" library="sym" deviceset="CAP" device="" value="100n"/>
<part name="C3" library="sym" deviceset="CAP" device="" value="100n"/>
</parts>
<sheets>
<sheet>
<plain>
<text x="136.8425" y="5.08" size="2.8448" layer="97" align="center-left">\!.tran .1 uic</text>
<text x="-15.24" y="13.97" size="2.8448" layer="97" align="center-left">\!K1 L1 L2 L3 L4 1</text>
<text x="-52.07" y="2.54" size="2.8448" layer="94" align="center-left">;isolated Out to controller+</text>
<text x="-60.96" y="-27.94" size="2.8448" layer="94" align="center-left">;isolated Out to controller-</text>
<text x="45.72" y="10.16" size="2.8448" layer="94" align="center-left">;DAC CHANNEL 1 OUT</text>
<text x="125.73" y="25.4" size="2.8448" layer="94" align="center-left">;To 1.65V needed by input circuit</text>
<text x="-24.13" y="-80.01" size="2.8448" layer="94" align="center-left">;Repeat circuit for Channel 2, use another 1.65V buffer amplifier at avoid hitting its current limit</text>
<text x="11.43" y="39.37" size="2.8448" layer="94" align="center-left">;To add gain if needed</text>
<text x="-57.15" y="95.25" size="2.8448" layer="94" align="center-left">;Op amps need some room on the upper end of the supply, hence 5V, these op-amps have a max rating of 5.5V</text>
<text x="26.67" y="-6.35" size="2.8448" layer="94" align="center-left">;Models Rout of the unbuffered DAC</text>
<text x="-24.13" y="-85.09" size="2.8448" layer="94" align="center-left">;Note:  should probably disbale the microcontroller's onboard DAC buffer.  Looks like signal will clip at 0.2V by the datasheet</text>
<text x="-25.4" y="-91.44" size="2.8448" layer="94" align="center-left">;Transformer Part #:  Mouser 710-760390015, Midcom/Wurth 760390015</text>
</plain>
<instances>
<instance part="V0_1" gate="G$1" x="45.72" y="-25.4"/>
<instance part="V0_2" gate="G$1" x="-22.86" y="-43.18"/>
<instance part="V0_3" gate="G$1" x="22.86" y="-40.64"/>
<instance part="V0_4" gate="G$1" x="33.02" y="-71.12"/>
<instance part="V0_5" gate="G$1" x="30.48" y="5.08" rot="R270"/>
<instance part="V0_6" gate="G$1" x="20.32" y="-68.58" rot="R270"/>
<instance part="V0_7" gate="G$1" x="48.26" y="63.5" rot="R90"/>
<instance part="V0_8" gate="G$1" x="76.2" y="63.5" rot="R90"/>
<instance part="V0_9" gate="G$1" x="104.14" y="-25.4" rot="R180"/>
<instance part="V0_10" gate="G$1" x="142.24" y="-25.4" rot="R180"/>
<instance part="V0_11" gate="G$1" x="50.8" y="25.4" rot="R90"/>
<instance part="V0_12" gate="G$1" x="30.48" y="66.04" rot="R90"/>
<instance part="V0_13" gate="G$1" x="63.5" y="66.04" rot="R90"/>
<instance part="L1" gate="G$1" x="-15.24" y="2.54" smashed="yes">
<attribute name="NAME" x="-9.525" y="-3.81" size="1.778" layer="95" align="center-left"/>
<attribute name="VALUE" x="-9.525" y="-10.16" size="1.778" layer="96" align="center-left"/>
<attribute name="SPICETYPE" x="-15.24" y="2.54" size="1.778" layer="96" align="center-left"/>
<attribute name="SPICELINE" x="-15.24" y="2.54" size="1.778" layer="96" align="center-left"/>
</instance>
<instance part="L2" gate="G$1" x="20.32" y="2.54" smashed="yes" rot="MR0">
<attribute name="NAME" x="14.605" y="-3.81" size="1.778" layer="95" rot="MR0" align="center-left"/>
<attribute name="VALUE" x="14.605" y="-10.16" size="1.778" layer="96" rot="MR0" align="center-left"/>
<attribute name="SPICETYPE" x="20.32" y="2.54" size="1.778" layer="96" rot="MR0" align="center-left"/>
<attribute name="SPICELINE" x="20.32" y="2.54" size="1.778" layer="96" rot="MR0" align="center-left"/>
</instance>
<instance part="L3" gate="G$1" x="-15.24" y="-15.24" smashed="yes">
<attribute name="NAME" x="-9.525" y="-21.59" size="1.778" layer="95" align="center-left"/>
<attribute name="VALUE" x="-9.525" y="-27.94" size="1.778" layer="96" align="center-left"/>
<attribute name="SPICETYPE" x="-15.24" y="-15.24" size="1.778" layer="96" align="center-left"/>
<attribute name="SPICELINE" x="-15.24" y="-15.24" size="1.778" layer="96" align="center-left"/>
</instance>
<instance part="L4" gate="G$1" x="20.32" y="-15.24" smashed="yes" rot="MR0">
<attribute name="NAME" x="14.605" y="-21.59" size="1.778" layer="95" rot="MR0" align="center-left"/>
<attribute name="VALUE" x="14.605" y="-27.94" size="1.778" layer="96" rot="MR0" align="center-left"/>
<attribute name="SPICETYPE" x="20.32" y="-15.24" size="1.778" layer="96" rot="MR0" align="center-left"/>
<attribute name="SPICELINE" x="20.32" y="-15.24" size="1.778" layer="96" rot="MR0" align="center-left"/>
</instance>
<instance part="R1" gate="G$1" x="-25.4" y="-27.94" smashed="yes">
<attribute name="NAME" x="-19.685" y="-34.29" size="1.778" layer="95" align="center-left"/>
<attribute name="VALUE" x="-19.685" y="-40.005" size="1.778" layer="96" align="center-left"/>
</instance>
<instance part="R2" gate="G$1" x="30.48" y="-43.18" smashed="yes">
<attribute name="NAME" x="36.195" y="-49.53" size="1.778" layer="95" align="center-left"/>
<attribute name="VALUE" x="36.195" y="-55.245" size="1.778" layer="96" align="center-left"/>
</instance>
<instance part="R3" gate="G$1" x="30.48" y="-55.88" smashed="yes">
<attribute name="NAME" x="36.195" y="-62.23" size="1.778" layer="95" align="center-left"/>
<attribute name="VALUE" x="36.195" y="-67.945" size="1.778" layer="96" align="center-left"/>
</instance>
<instance part="C1" gate="G$1" x="17.78" y="-58.42" smashed="yes">
<attribute name="NAME" x="21.59" y="-59.69" size="1.778" layer="95" align="center-left"/>
<attribute name="VALUE" x="21.59" y="-67.31" size="1.778" layer="96" align="center-left"/>
</instance>
<instance part="V2" gate="G$1" x="76.2" y="78.74" smashed="yes">
<attribute name="NAME" x="80.01" y="76.2" size="1.778" layer="95" align="center-left"/>
<attribute name="VALUE" x="80.01" y="54.61" size="1.778" layer="96"/>
</instance>
<instance part="V3" gate="G$1" x="48.26" y="78.74" smashed="yes">
<attribute name="NAME" x="52.07" y="76.2" size="1.778" layer="95" align="center-left"/>
<attribute name="VALUE" x="52.07" y="54.61" size="1.778" layer="96"/>
</instance>
<instance part="V4" gate="G$1" x="104.14" y="-10.16" smashed="yes">
<attribute name="NAME" x="107.95" y="-12.7" size="1.778" layer="95" align="center-left"/>
<attribute name="VALUE" x="107.95" y="-34.29" size="1.778" layer="96"/>
</instance>
<instance part="V5" gate="G$1" x="142.24" y="-10.16" smashed="yes">
<attribute name="NAME" x="146.05" y="-12.7" size="1.778" layer="95" align="center-left"/>
<attribute name="VALUE" x="146.05" y="-34.29" size="1.778" layer="96"/>
</instance>
<instance part="B1" gate="G$1" x="45.72" y="-10.16" smashed="yes">
<attribute name="NAME" x="49.53" y="-12.7" size="1.778" layer="95" align="center-left"/>
<attribute name="VALUE" x="49.53" y="-25.4" size="1.778" layer="96" align="center-left"/>
</instance>
<instance part="R4" gate="G$1" x="38.1" y="33.02" smashed="yes" rot="R270">
<attribute name="NAME" x="29.21" y="33.02" size="1.778" layer="95" align="bottom-center"/>
<attribute name="VALUE" x="29.21" y="27.94" size="1.778" layer="96" align="top-center"/>
<attribute name="SPICELINE" x="38.1" y="33.02" size="1.778" layer="96" align="bottom-center" display="off"/>
</instance>
<instance part="R5" gate="G$1" x="53.34" y="27.94" smashed="yes" rot="R270">
<attribute name="NAME" x="44.45" y="27.94" size="1.778" layer="95" align="bottom-center"/>
<attribute name="VALUE" x="44.45" y="22.86" size="1.778" layer="96" align="top-center"/>
</instance>
<instance part="R6" gate="G$1" x="7.62" y="-35.56" smashed="yes">
<attribute name="NAME" x="13.335" y="-41.91" size="1.778" layer="95" align="center-left"/>
<attribute name="VALUE" x="13.335" y="-47.625" size="1.778" layer="96" align="center-left"/>
</instance>
<instance part="R7" gate="G$1" x="43.18" y="10.16" smashed="yes">
<attribute name="NAME" x="48.895" y="3.81" size="1.778" layer="95" align="center-left"/>
<attribute name="VALUE" x="48.895" y="-1.905" size="1.778" layer="96" align="center-left"/>
</instance>
<instance part="C2" gate="G$1" x="27.94" y="76.2" smashed="yes">
<attribute name="NAME" x="31.75" y="74.93" size="1.778" layer="95" align="center-left"/>
<attribute name="VALUE" x="31.75" y="67.31" size="1.778" layer="96" align="center-left"/>
</instance>
<instance part="C3" gate="G$1" x="60.96" y="76.2" smashed="yes">
<attribute name="NAME" x="64.77" y="74.93" size="1.778" layer="95" align="center-left"/>
<attribute name="VALUE" x="64.77" y="67.31" size="1.778" layer="96" align="center-left"/>
</instance>
</instances>
<busses>
</busses>
<nets>
<net name="0" class="0">
<segment>
<pinref part="V0_2" gate="G$1" pin="0"/>
<pinref part="R1" gate="G$1" pin="B"/>
</segment>
<segment>
<pinref part="V0_4" gate="G$1" pin="0"/>
<pinref part="R3" gate="G$1" pin="B"/>
</segment>
<segment>
<pinref part="V0_6" gate="G$1" pin="0"/>
<pinref part="C1" gate="G$1" pin="B"/>
</segment>
<segment>
<pinref part="V0_8" gate="G$1" pin="0"/>
<pinref part="V2" gate="G$1" pin="-"/>
</segment>
<segment>
<pinref part="V0_7" gate="G$1" pin="0"/>
<pinref part="V3" gate="G$1" pin="-"/>
</segment>
<segment>
<pinref part="V0_9" gate="G$1" pin="0"/>
<pinref part="V4" gate="G$1" pin="-"/>
</segment>
<segment>
<pinref part="V0_10" gate="G$1" pin="0"/>
<pinref part="V5" gate="G$1" pin="-"/>
</segment>
<segment>
<pinref part="V0_1" gate="G$1" pin="0"/>
<pinref part="B1" gate="G$1" pin="-"/>
</segment>
<segment>
<pinref part="V0_11" gate="G$1" pin="0"/>
<pinref part="R5" gate="G$1" pin="A"/>
</segment>
<segment>
<pinref part="V0_12" gate="G$1" pin="0"/>
<pinref part="C2" gate="G$1" pin="B"/>
</segment>
<segment>
<pinref part="V0_13" gate="G$1" pin="0"/>
<pinref part="C3" gate="G$1" pin="B"/>
</segment>
</net>
<net name="N$1" class="0">
<segment>
<pinref part="R2" gate="G$1" pin="B"/>
<pinref part="R3" gate="G$1" pin="A"/>
<pinref part="C1" gate="G$1" pin="A"/>
<wire x1="20.32" y1="-58.42" x2="20.32" y2="-45.72" width="0.1524" layer="91"/>
<wire x1="33.02" y1="-58.42" x2="20.32" y2="-58.42" width="0.1524" layer="91"/>
<junction x="33.02" y="-58.42"/>
<junction x="20.32" y="-58.42"/>
</segment>
</net>
<net name="N$2" class="0">
<segment>
<wire x1="48.26" y1="81.28" x2="48.26" y2="86.36" width="0.1524" layer="91"/>
<wire x1="48.26" y1="81.28" x2="30.48" y2="81.28" width="0.1524" layer="91"/>
<pinref part="C2" gate="G$1" pin="A"/>
<wire x1="30.48" y1="76.2" x2="30.48" y2="81.28" width="0.1524" layer="91"/>
<pinref part="V3" gate="G$1" pin="+"/>
<wire x1="48.26" y1="76.2" x2="48.26" y2="81.28" width="0.1524" layer="91"/>
<junction x="48.26" y="81.28"/>
</segment>
</net>
<net name="N$3" class="0">
<segment>
<wire x1="76.2" y1="81.28" x2="76.2" y2="86.36" width="0.1524" layer="91"/>
<wire x1="76.2" y1="81.28" x2="63.5" y2="81.28" width="0.1524" layer="91"/>
<pinref part="C3" gate="G$1" pin="A"/>
<wire x1="63.5" y1="76.2" x2="63.5" y2="81.28" width="0.1524" layer="91"/>
<pinref part="V2" gate="G$1" pin="+"/>
<wire x1="76.2" y1="76.2" x2="76.2" y2="81.28" width="0.1524" layer="91"/>
<junction x="76.2" y="81.28"/>
</segment>
</net>
<net name="N$4" class="0">
<segment>
<pinref part="R4" gate="G$1" pin="B"/>
<wire x1="22.86" y1="30.48" x2="17.78" y2="30.48" width="0.1524" layer="91"/>
<wire x1="17.78" y1="10.16" x2="17.78" y2="30.48" width="0.1524" layer="91"/>
<wire x1="25.4" y1="10.16" x2="17.78" y2="10.16" width="0.1524" layer="91"/>
<pinref part="L2" gate="G$1" pin="A"/>
<wire x1="17.78" y1="0" x2="17.78" y2="10.16" width="0.1524" layer="91"/>
<junction x="17.78" y="10.16"/>
</segment>
</net>
<net name="N$5" class="0">
<segment>
<pinref part="R4" gate="G$1" pin="A"/>
<wire x1="38.1" y1="30.48" x2="35.56" y2="30.48" width="0.1524" layer="91"/>
<pinref part="R5" gate="G$1" pin="B"/>
<wire x1="38.1" y1="25.4" x2="38.1" y2="30.48" width="0.1524" layer="91"/>
<wire x1="38.1" y1="12.7" x2="38.1" y2="25.4" width="0.1524" layer="91"/>
<junction x="38.1" y="25.4"/>
<wire x1="38.1" y1="12.7" x2="35.56" y2="12.7" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$6" class="0">
<segment>
<wire x1="167.64" y1="22.86" x2="93.98" y2="22.86" width="0.1524" layer="91"/>
<wire x1="93.98" y1="-33.02" x2="93.98" y2="22.86" width="0.1524" layer="91"/>
<pinref part="L4" gate="G$1" pin="B"/>
<wire x1="17.78" y1="-33.02" x2="17.78" y2="-30.48" width="0.1524" layer="91"/>
<wire x1="17.78" y1="-33.02" x2="10.16" y2="-33.02" width="0.1524" layer="91"/>
<wire x1="93.98" y1="-33.02" x2="17.78" y2="-33.02" width="0.1524" layer="91"/>
<junction x="17.78" y="-33.02"/>
<wire x1="17.78" y1="-35.56" x2="17.78" y2="-33.02" width="0.1524" layer="91"/>
<pinref part="R6" gate="G$1" pin="A"/>
<wire x1="10.16" y1="-38.1" x2="10.16" y2="-33.02" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$7" class="0">
<segment>
<pinref part="R7" gate="G$1" pin="A"/>
<wire x1="45.72" y1="7.62" x2="35.56" y2="7.62" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$8" class="0">
<segment>
<pinref part="L1" gate="G$1" pin="A"/>
<wire x1="-12.7" y1="0" x2="-50.8" y2="0" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$9" class="0">
<segment>
<pinref part="B1" gate="G$1" pin="+"/>
<pinref part="R7" gate="G$1" pin="B"/>
<wire x1="45.72" y1="-12.7" x2="45.72" y2="-5.08" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$10" class="0">
<segment>
<pinref part="V4" gate="G$1" pin="+"/>
<wire x1="104.14" y1="-12.7" x2="104.14" y2="-2.54" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$11" class="0">
<segment>
<pinref part="V5" gate="G$1" pin="+"/>
<wire x1="142.24" y1="-12.7" x2="142.24" y2="-5.08" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$12" class="0">
<segment>
<pinref part="L3" gate="G$1" pin="A"/>
<pinref part="L1" gate="G$1" pin="B"/>
<wire x1="-12.7" y1="-17.78" x2="-12.7" y2="-12.7" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$13" class="0">
<segment>
<pinref part="L4" gate="G$1" pin="A"/>
<pinref part="L2" gate="G$1" pin="B"/>
<wire x1="17.78" y1="-17.78" x2="17.78" y2="-12.7" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$14" class="0">
<segment>
<pinref part="R1" gate="G$1" pin="A"/>
<wire x1="-22.86" y1="-30.48" x2="-50.8" y2="-30.48" width="0.1524" layer="91"/>
<pinref part="L3" gate="G$1" pin="B"/>
<wire x1="-12.7" y1="-30.48" x2="-22.86" y2="-30.48" width="0.1524" layer="91"/>
<junction x="-22.86" y="-30.48"/>
</segment>
</net>
<net name="N$16" class="0">
<segment>
<wire x1="12.7" y1="-40.64" x2="5.08" y2="-40.64" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$17" class="0">
<segment>
<pinref part="R2" gate="G$1" pin="A"/>
<wire x1="38.1" y1="-45.72" x2="33.02" y2="-45.72" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$18" class="0">
<segment>
<wire x1="15.24" y1="-50.8" x2="15.24" y2="-45.72" width="0.1524" layer="91"/>
<pinref part="R6" gate="G$1" pin="B"/>
<wire x1="15.24" y1="-50.8" x2="10.16" y2="-50.8" width="0.1524" layer="91"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
</eagle>
