.global ctr_font_data_begin, ctr_font_data_end, ctr_font_data_size

.section .rodata.default_font

.align 4
ctr_font_data_begin:
.word ctr_font_begin

.align 4
ctr_font_data_end:
.word ctr_font_end

.align 4
ctr_font_begin:
.incbin "font.pcf"
ctr_font_end:

.align 4
ctr_font_data_size:
.word ctr_font_end-ctr_font_begin

