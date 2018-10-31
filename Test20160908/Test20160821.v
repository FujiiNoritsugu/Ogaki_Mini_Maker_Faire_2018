// Copyright (C) 1991-2015 Altera Corporation. All rights reserved.
// Your use of Altera Corporation's design tools, logic functions 
// and other software and tools, and its AMPP partner logic 
// functions, and any output files from any of the foregoing 
// (including device programming or simulation files), and any 
// associated documentation or information are expressly subject 
// to the terms and conditions of the Altera Program License 
// Subscription Agreement, the Altera Quartus II License Agreement,
// the Altera MegaCore Function License Agreement, or other 
// applicable license agreement, including, without limitation, 
// that your use is for the sole purpose of programming logic 
// devices manufactured by Altera and sold by Altera or its 
// authorized distributors.  Please refer to the applicable 
// agreement for further details.

// PROGRAM		"Quartus II 64-Bit"
// VERSION		"Version 15.0.1 Build 150 06/03/2015 SJ Web Edition"
// CREATED		"Wed Feb 17 04:40:17 2016"

module Test20160821(
	clk,
	reset,
	pin0,
	pin1,
	sdo,
	convst,
	sck,
	sdi,
	debug_convst,
	debug_sck,
	debug_sdi,
	debug_mb,
	debug_irq,
 memory_mem_a,         //        memory.mem_a
 memory_mem_ba,        //              .mem_ba
 memory_mem_ck,        //              .mem_ck
 memory_mem_ck_n,      //              .mem_ck_n
 memory_mem_cke,       //              .mem_cke
 memory_mem_cs_n,      //              .mem_cs_n
 memory_mem_ras_n,     //              .mem_ras_n
 memory_mem_cas_n,     //              .mem_cas_n
 memory_mem_we_n,      //              .mem_we_n
 memory_mem_reset_n,   //              .mem_reset_n
 memory_mem_dq,        //              .mem_dq
 memory_mem_dqs,       //              .mem_dqs
 memory_mem_dqs_n,     //              .mem_dqs_n
 memory_mem_odt,       //              .mem_odt
 memory_mem_dm,        //              .mem_dm
 memory_oct_rzqin     //              .oct_rzqin	
);

input wire	clk;
input wire	reset;
output wire pin0;
output wire pin1;
input wire	sdo;
output wire	convst;
output wire	sck;
output wire	sdi;
output wire	debug_convst;
output wire	debug_sck;
output wire	debug_sdi;
output wire	debug_mb;
output wire debug_irq;
output wire [12:0] memory_mem_a;         //        memory.mem_a
output wire [2:0]  memory_mem_ba;        //              .mem_ba
output wire        memory_mem_ck;        //              .mem_ck
output wire        memory_mem_ck_n;      //              .mem_ck_n
output wire        memory_mem_cke;       //              .mem_cke
output wire        memory_mem_cs_n;      //              .mem_cs_n
output wire        memory_mem_ras_n;     //              .mem_ras_n
output wire        memory_mem_cas_n;     //              .mem_cas_n
output wire        memory_mem_we_n;     //              .mem_we_n
output wire        memory_mem_reset_n;   //              .mem_reset_n
inout  wire [7:0]  memory_mem_dq;        //              .mem_dq
inout  wire        memory_mem_dqs;       //              .mem_dqs
inout  wire        memory_mem_dqs_n;     //              .mem_dqs_n
output wire        memory_mem_odt;       //              .mem_odt
output wire        memory_mem_dm;        //              .mem_dm
input  wire        memory_oct_rzqin;     //              .oct_rzqin

// on chip
wire [12:0] onchip_address;           //       onchip_memory2_0_s2.address
wire        onchip_chipselect;        //                          .chipselect
wire        onchip_clken;             //                          .clken
wire        onchip_write;             //                          .write
wire [31:0] onchip_readdata;          //                          .readdata
wire [31:0] onchip_writedata;         //                          .writedata
wire [3:0]  onchip_byteenable;        //                          .byteenable
wire		   irq_flg;
reg [12:0] reg_onchip_address;           //       onchip_memory2_0_s2.address
reg        reg_onchip_chipselect;        //                          .chipselect
reg        reg_onchip_clken;             //                          .clken
reg        reg_onchip_write;             //                          .write
reg [31:0] reg_onchip_readdata;          //                          .readdata
reg [31:0] reg_onchip_writedata;         //                          .writedata
reg [3:0]  reg_onchip_byteenable;        //                          .byteenable
reg          reg_irq_flg;
//reg          reg_data_store_flg = 1'b0;

assign onchip_address = reg_onchip_address;
assign onchip_chipselect = reg_onchip_chipselect;
assign onchip_write = reg_onchip_write;
assign onchip_writedata = reg_onchip_writedata;
assign onchip_byteenable = reg_onchip_byteenable;
assign onchip_clken = reg_onchip_clken;
assign irq_flg = reg_irq_flg;

reg [31:0]enable_counter = 32'h0;
reg [11:0]address_counter = 12'h0;

always @(posedge clk) begin
    if(enable_counter > 12'hc350) begin
			enable_counter <= 32'h0;
			address_counter <= address_counter + 1'b1;
		if(address_counter == 12'd1000)  begin
			address_counter <= 12'h0;
		end
	  end
	  else begin
			reg_onchip_chipselect <= 1'b1;
			reg_onchip_clken <= 1'b1;
	      reg_onchip_byteenable <= 4'b1111;
		   reg_onchip_address <= address_counter;
			reg_onchip_writedata[11:0] <= SYNTHESIZED_WIRE_1;
			reg_onchip_writedata[23:12] <= SYNTHESIZED_WIRE_2;
			if(SYNTHESIZED_WIRE_1 > 12'd500)
				reg_irq_flg <= 1'b1;
			else
				reg_irq_flg <= 1'b0;
		  	reg_onchip_write <= 1'b1;
			enable_counter <= enable_counter + 1'b1;
	end
end

wire	SYNTHESIZED_WIRE_0;
wire	[11:0] SYNTHESIZED_WIRE_1;
wire	[11:0] SYNTHESIZED_WIRE_2;
wire	SYNTHESIZED_WIRE_7;
wire	[7:0] SYNTHESIZED_WIRE_4;
wire	[7:0] SYNTHESIZED_WIRE_6;

ADC_Reader	b2v_inst(
	.sdo(sdo),
	.clk(SYNTHESIZED_WIRE_0),
	.rst(reset),
	.convst(convst),
	.sck(sck),
	.sdi(sdi),
	.debug_convst(debug_convst),
	.debug_sck(debug_sck),
	.debug_sdi(debug_sdi),
	.debug_mb(debug_mb),
	.debug_sdo1(SYNTHESIZED_WIRE_1),
	.debug_sdo2(SYNTHESIZED_WIRE_2));
	defparam	b2v_inst.TCYC = 24'b000000000000111110100000;

adc_50_to_40	b2v_inst1(
	.refclk(clk),
	.rst(reset),
	.outclk_0(SYNTHESIZED_WIRE_0));

Test20160908_sys    b2v_inst2(
	.clk_clk(clk),
   .memory_mem_a(memory_mem_a),
   .memory_mem_ba(memory_mem_ba),
   .memory_mem_ck(memory_mem_ck),
   .memory_mem_ck_n(memory_mem_ck_n),
   .memory_mem_cke(memory_mem_cke),
   .memory_mem_cs_n(memory_mem_cs_n),
   .memory_mem_ras_n(memory_mem_ras_n),
   .memory_mem_cas_n(memory_mem_cas_n),
   .memory_mem_we_n(memory_mem_we_n),
   .memory_mem_reset_n(memory_mem_reset_n),
   .memory_mem_dq(memory_mem_dq),
   .memory_mem_dqs(memory_mem_dqs),
   .memory_mem_dqs_n(memory_mem_dqs_n),
   .memory_mem_odt(memory_mem_odt),
   .memory_mem_dm(memory_mem_dm),
   .memory_oct_rzqin(memory_oct_rzqin),
	.memory_socket_0_conduit_end_range1(SYNTHESIZED_WIRE_4),  // memory_socket_0_conduit_end.range1
	.memory_socket_0_conduit_end_range2(SYNTHESIZED_WIRE_6),  //                            .range2
	.memory_socket_0_conduit_end_irq_flg(irq_flg), //                            .irq_flg
	.memory_socket_0_conduit_end_debug(debug_irq),
  .onchip_memory2_0_s2_address(onchip_address),           //       onchip_memory2_0_s2.address
  .onchip_memory2_0_s2_chipselect(onchip_chipselect),        //                          .chipselect
  .onchip_memory2_0_s2_clken(onchip_clken),             //                          .clken
  .onchip_memory2_0_s2_write(onchip_write),             //                          .write
  .onchip_memory2_0_s2_readdata(onchip_readdata),          //                          .readdata
  .onchip_memory2_0_s2_writedata(onchip_writedata),         //                          .writedata
  .onchip_memory2_0_s2_byteenable(onchip_byteenable),        //                          .byteenable
    .reset_reset_n(~reset)
   );
	
Servo	b2v_inst4(
	.clkin(SYNTHESIZED_WIRE_7),
	.rstn(reset),
	.cntin(SYNTHESIZED_WIRE_4),
	.pwmout(pin1));

Servo	b2v_inst5(
	.clkin(SYNTHESIZED_WIRE_7),
	.rstn(reset),
	.cntin(SYNTHESIZED_WIRE_6),
	.pwmout(pin0));

Clock50kHz	b2v_inst6(
	.clockin50mHz(clk),
	.clockout(SYNTHESIZED_WIRE_7));

endmodule
