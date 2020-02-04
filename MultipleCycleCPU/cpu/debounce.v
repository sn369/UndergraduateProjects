`timescale 1ns / 1ps
module debounce (clk,key,key_pulse);
	input clk;
    input key;                        //����İ���					
	output key_pulse;                  //������������������	
 
    reg key_rst_pre;                //����һ���Ĵ����ͱ����洢��һ������ʱ�İ���ֵ
    reg key_rst;                    //����һ���Ĵ����������洢��ǰʱ�̴����İ���ֵ
 
    wire key_edge;                   //��⵽�����ɸߵ��ͱ仯�ǲ���һ��������
 
    //���÷�������ֵ�ص㣬������ʱ�Ӵ���ʱ����״̬�洢�������Ĵ���������
    always @(posedge clk) begin
        key_rst <= key;                     //��һ��ʱ�������ش���֮��key��ֵ����key_rst,ͬʱkey_rst��ֵ����key_rst_pre
        key_rst_pre <= key_rst;             //��������ֵ���൱�ھ�������ʱ�Ӵ�����key_rst�洢���ǵ�ǰʱ��key��ֵ��key_rst_pre�洢����ǰһ��ʱ�ӵ�key��ֵ   
    end
 
    assign key_edge = key_rst_pre & (~key_rst);//������ؼ�⡣��key��⵽�½���ʱ��key_edge����һ��ʱ�����ڵĸߵ�ƽ
 
    reg	[17:0] cnt;                       //������ʱ���õļ�������ϵͳʱ��12MHz��Ҫ��ʱ20ms����ʱ�䣬������Ҫ18λ������     
 
    //����20ms��ʱ������⵽key_edge��Ч�Ǽ��������㿪ʼ����
    always @(posedge clk) begin
        if(key_edge)
            cnt <= 18'h0;
        else
            cnt <= cnt + 1'h1;
    end  
 
    reg key_sec_pre;                //��ʱ�����ƽ�Ĵ�������
    reg key_sec;                    
 
    //��ʱ����key���������״̬��Ͳ���һ��ʱ�ӵĸ����塣�������״̬�ǸߵĻ�˵��������Ч
    always @(posedge clk)
        if (cnt==18'h3ffff)
            key_sec <= key;  
            
    always @(posedge clk)           
        key_sec_pre <= key_sec;      
        
    assign  key_pulse = key_sec_pre & (~key_sec);   
    
endmodule