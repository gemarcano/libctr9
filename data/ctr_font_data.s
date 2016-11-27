.global ctr_font_data_begin, ctr_font_data_end, ctr_font_data_size

.section .rodata.default_font

ctr_font_data_begin:
.incbin "font.pcf"
ctr_font_data_end:

ctr_font_data_size:
.word ctr_font_data_end-ctr_font_data_begin

