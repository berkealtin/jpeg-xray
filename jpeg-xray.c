    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    int main(int argc, char* argv[])
    {
        if (argc != 2)
        {
            printf("Hatali kullanim. Lutfen dosyayi da yazin.\n");
            return 1;
        }

        FILE* dosya = fopen(argv[1], "rb");

        if (dosya == NULL)
        {
            printf("Hata: Dosya bulunamadi veya acilamadi!\n");
            return 1;
        }

        int ilkbyte = fgetc(dosya);
        int ikincibyte = fgetc(dosya);

        if (ilkbyte != 0xFF || ikincibyte != 0xD8)
        {
            printf("Hata: Bu dosya bir JPEG formatinda degil!\n");
            fclose(dosya);
            return 1;
        }

        int out_width = -1;
        int out_height = -1;
        int out_x_density = -1;
        int out_y_density = -1;
        int out_unit = -1;
        int has_icc = 0;
        int has_xmp = 0;

        char out_make[100] = {'\0'};
        char out_model[100] = {'\0'};
        char out_date[100] = {'\0'};         
        char out_modify_date[100] = {'\0'};  
        char out_software[100] = {'\0'};
        char out_comment[255] = {'\0'};

        char out_iso[20] = {'\0'};
        char out_exposure[20] = {'\0'};
        char out_fnumber[20] = {'\0'};
        char xmp_creator[100] = {'\0'};
        char xmp_date[100] = {'\0'};
        char xmp_label[100] = {'\0'};

        printf("\n[Segments Map]\n");
        printf("FFD8: SOI (Start of Image)\n");

        while (1)
        {
            int m1 = fgetc(dosya);
            int m2 = fgetc(dosya);

            if (m1 != 0xFF)
            {
                break;
            }
            if (m2 == 0xE0)
            {
                printf("FFE0: APP0 (JFIF)\n");
            }
            else if (m2 == 0xE1)
            {
                printf("FFE1: APP1 (EXIF/XMP)\n");
            }
            else if (m2 == 0xE2)
            {
                printf("FFE2: APP2 (ICC Profile)\n");
            }
            else if (m2 == 0xDB)
            {
                printf("FFDB: DQT (Quantization Table)\n");
            }
            else if (m2 == 0xC0)
            {
                printf("FFC0: SOF0 (Start of Frame)\n");
            }
            else if (m2 == 0xC4)
            {
                printf("FFC4: DHT (Huffman Table)\n");
            }
            else if (m2 == 0xFE)
            {
                printf("FFFE: COM (Comment)\n");
            }
            else if (m2 == 0xDA)
            {
                printf("FFDA: SOS (Start of Scan)\n");
                break;
            }

            int len1 = fgetc(dosya);
            int len2 = fgetc(dosya);
            int uzunluk = (len1 << 8) | len2;

            if (m2 == 0xC0)
            {
                fgetc(dosya); 
                int h1 = fgetc(dosya);
                int h2 = fgetc(dosya);
                out_height = (h1 << 8) | h2;

                int w1 = fgetc(dosya);
                int w2 = fgetc(dosya);
                out_width = (w1 << 8) | w2;

                fseek(dosya, uzunluk - 2 - 5, SEEK_CUR);
            }
            else if (m2 == 0xFE)
            {
                int idx = 0;
                for (int i = 0; i < uzunluk - 2; i++)
                {
                    int c = fgetc(dosya);
                    if (idx < 254 && c != '\0')
                    {
                        out_comment[idx] = c;
                        idx++;
                    }
                }
                out_comment[idx] = '\0';
            }
            else if (m2 == 0xE2)
            {
                has_icc = 1;
                fseek(dosya, uzunluk - 2, SEEK_CUR);
            }
            else if (m2 == 0xE1)
            {
                long e1_start = ftell(dosya);
                char e_head[30] = {'\0'};
                
                for (int i = 0; i < 29 && i < (uzunluk - 2); i++)
                {
                    e_head[i] = fgetc(dosya);
                }

                if (strstr(e_head, "adobe") != NULL)
                {
                    has_xmp = 1;
                    char* xmp_data = (char*)malloc(uzunluk);
                    
                    if (xmp_data != NULL)
                    {
                        fseek(dosya, e1_start, SEEK_SET);
                        for (int i = 0; i < uzunluk - 2; i++)
                        {
                            xmp_data[i] = fgetc(dosya);
                        }
                        xmp_data[uzunluk - 2] = '\0';

                        char* p1 = strstr(xmp_data, "CreatorTool=\"");
                        if (p1 != NULL)
                        {
                            p1 = p1 + 13;
                            int idx = 0;
                            while (*p1 != '"' && *p1 != '\0' && idx < 99)
                            {
                                xmp_creator[idx] = *p1;
                                p1++;
                                idx++;
                            }
                            xmp_creator[idx] = '\0';
                        }

                        char* p2 = strstr(xmp_data, "MetadataDate=\"");
                        if (p2 != NULL)
                        {
                            p2 = p2 + 14;
                            int idx = 0;
                            while (*p2 != '"' && *p2 != '\0' && idx < 99)
                            {
                                xmp_date[idx] = *p2;
                                p2++;
                                idx++;
                            }
                            xmp_date[idx] = '\0';
                        }

                        char* p3 = strstr(xmp_data, "Label=\"");
                        if (p3 != NULL)
                        {
                            p3 = p3 + 7;
                            int idx = 0;
                            while (*p3 != '"' && *p3 != '\0' && idx < 99)
                            {
                                xmp_label[idx] = *p3;
                                p3++;
                                idx++;
                            }
                            xmp_label[idx] = '\0';
                        }
                        free(xmp_data);
                    }
                    fseek(dosya, e1_start + uzunluk - 2, SEEK_SET);
                }
                else if (e_head[0] == 'E' && e_head[1] == 'x' && e_head[2] == 'i' && e_head[3] == 'f')
                {
                    fseek(dosya, e1_start + 6, SEEK_SET);
                    long tiff_baslangic = ftell(dosya);
                    
                    int b1 = fgetc(dosya);
                    int b2 = fgetc(dosya);
                    int is_le = 0;

                    if (b1 == 0x49)
                    {
                        is_le = 1;
                    }

                    fseek(dosya, 6, SEEK_CUR);

                    int t1 = fgetc(dosya);
                    int t2 = fgetc(dosya);
                    int tag_sayisi = 0;

                    if (is_le)
                    {
                        tag_sayisi = (t2 << 8) | t1;
                    }
                    else
                    {
                        tag_sayisi = (t1 << 8) | t2;
                    }

                    int sub_ifd_offset = 0;

                    for (int i = 0; i < tag_sayisi; i++)
                    {
                        int id1 = fgetc(dosya);
                        int id2 = fgetc(dosya);
                        int tag_id = 0;

                        if (is_le)
                        {
                            tag_id = (id2 << 8) | id1;
                        }
                        else
                        {
                            tag_id = (id1 << 8) | id2;
                        }

                        int dt1 = fgetc(dosya);
                        int dt2 = fgetc(dosya);
                        int d_type = 0;

                        if (is_le)
                        {
                            d_type = (dt2 << 8) | dt1;
                        }
                        else
                        {
                            d_type = (dt1 << 8) | dt2;
                        }

                        int cc1 = fgetc(dosya);
                        int cc2 = fgetc(dosya);
                        int cc3 = fgetc(dosya);
                        int cc4 = fgetc(dosya);
                        int c_count = 0;

                        if (is_le)
                        {
                            c_count = (cc4 << 24) | (cc3 << 16) | (cc2 << 8) | cc1;
                        }
                        else
                        {
                            c_count = (cc1 << 24) | (cc2 << 16) | (cc3 << 8) | cc4;
                        }

                        int v1 = fgetc(dosya);
                        int v2 = fgetc(dosya);
                        int v3 = fgetc(dosya);
                        int v4 = fgetc(dosya);
                        int off_val = 0;

                        if (is_le)
                        {
                            off_val = (v4 << 24) | (v3 << 16) | (v2 << 8) | v1;
                        }
                        else
                        {
                            off_val = (v1 << 24) | (v2 << 16) | (v3 << 8) | v4;
                        }

                        if (tag_id == 0x8769)
                        {
                            sub_ifd_offset = off_val;
                        }

                        if (d_type == 2)
                        {
                            if (tag_id == 0x010F || tag_id == 0x0110 || tag_id == 0x0132 || tag_id == 0x0131)
                            {
                                int idx = 0;
                                long geri = ftell(dosya);
                                fseek(dosya, tiff_baslangic + off_val, SEEK_SET);

                                for (int k = 0; k < c_count && k < 99; k++)
                                {
                                    int c = fgetc(dosya);
                                    if (c != '\0')
                                    {
                                        if (tag_id == 0x010F)
                                        {
                                            out_make[idx] = c;
                                        }
                                        else if (tag_id == 0x0110)
                                        {
                                            out_model[idx] = c;
                                        }
                                        else if (tag_id == 0x0132)
                                        {
                                            out_modify_date[idx] = c;
                                        }
                                        else if (tag_id == 0x0131)
                                        {
                                            out_software[idx] = c;
                                        }
                                        idx++;
                                    }
                                }

                                if (tag_id == 0x010F)
                                {
                                    out_make[idx] = '\0';
                                }
                                else if (tag_id == 0x0110)
                                {
                                    out_model[idx] = '\0';
                                }
                                else if (tag_id == 0x0132)
                                {
                                    out_modify_date[idx] = '\0';
                                }
                                else if (tag_id == 0x0131)
                                {
                                    out_software[idx] = '\0';
                                }

                                fseek(dosya, geri, SEEK_SET);
                            }
                        }
                    }

                    if (sub_ifd_offset > 0)
                    {
                        fseek(dosya, tiff_baslangic + sub_ifd_offset, SEEK_SET);
                        int s_t1 = fgetc(dosya);
                        int s_t2 = fgetc(dosya);
                        int s_count = 0;

                        if (is_le)
                        {
                            s_count = (s_t2 << 8) | s_t1;
                        }
                        else
                        {
                            s_count = (s_t1 << 8) | s_t2;
                        }

                        for (int j = 0; j < s_count; j++)
                        {
                            int id1 = fgetc(dosya);
                            int id2 = fgetc(dosya);
                            int s_tag_id = 0;

                            if (is_le)
                            {
                                s_tag_id = (id2 << 8) | id1;
                            }
                            else
                            {
                                s_tag_id = (id1 << 8) | id2;
                            }

                            int dt1 = fgetc(dosya);
                            int dt2 = fgetc(dosya);
                            
                            fseek(dosya, 4, SEEK_CUR);

                            int v1 = fgetc(dosya);
                            int v2 = fgetc(dosya);
                            int v3 = fgetc(dosya);
                            int v4 = fgetc(dosya);
                            int s_off = 0;

                            if (is_le)
                            {
                                s_off = (v4 << 24) | (v3 << 16) | (v2 << 8) | v1;
                            }
                            else
                            {
                                s_off = (v1 << 24) | (v2 << 16) | (v3 << 8) | v4;
                            }

                            if (s_tag_id == 0x8827)
                            {
                                int iso_val = 0;
                                if (is_le)
                                {
                                    iso_val = (v2 << 8) | v1;
                                }
                                else
                                {
                                    iso_val = (v1 << 8) | v2;
                                }
                                sprintf(out_iso, "%d", iso_val);
                            }
                            else if (s_tag_id == 0x829A || s_tag_id == 0x829D || s_tag_id == 0x9003)
                            {
                                long s_back = ftell(dosya);
                                fseek(dosya, tiff_baslangic + s_off, SEEK_SET);

                                if (s_tag_id == 0x9003)
                                {
                                    int idx = 0;
                                    for (int k = 0; k < 99; k++)
                                    {
                                        int c = fgetc(dosya);
                                        if (c != '\0')
                                        {
                                            out_date[idx] = c;
                                            idx++;
                                        }
                                        else
                                        {
                                            break;
                                        }
                                    }
                                    out_date[idx] = '\0';
                                }
                                else if (s_tag_id == 0x829A)
                                {
                                    unsigned int num1 = fgetc(dosya); unsigned int num2 = fgetc(dosya); unsigned int num3 = fgetc(dosya); unsigned int num4 = fgetc(dosya);
                                    unsigned int num = 0;
                                    if (is_le) { num = (num4<<24)|(num3<<16)|(num2<<8)|num1; } else { num = (num1<<24)|(num2<<16)|(num3<<8)|num4; }

                                    unsigned int den1 = fgetc(dosya); unsigned int den2 = fgetc(dosya); unsigned int den3 = fgetc(dosya); unsigned int den4 = fgetc(dosya);
                                    unsigned int den = 0;
                                    if (is_le) { den = (den4<<24)|(den3<<16)|(den2<<8)|den1; } else { den = (den1<<24)|(den2<<16)|(den3<<8)|den4; }

                                    sprintf(out_exposure, "%u/%u", num, den);
                                }
                                else if (s_tag_id == 0x829D)
                                {
                                    unsigned int num1 = fgetc(dosya); unsigned int num2 = fgetc(dosya); unsigned int num3 = fgetc(dosya); unsigned int num4 = fgetc(dosya);
                                    unsigned int num = 0;
                                    if (is_le) { num = (num4<<24)|(num3<<16)|(num2<<8)|num1; } else { num = (num1<<24)|(num2<<16)|(num3<<8)|num4; }

                                    unsigned int den1 = fgetc(dosya); unsigned int den2 = fgetc(dosya); unsigned int den3 = fgetc(dosya); unsigned int den4 = fgetc(dosya);
                                    unsigned int den = 0;
                                    if (is_le) { den = (den4<<24)|(den3<<16)|(den2<<8)|den1; } else { den = (den1<<24)|(den2<<16)|(den3<<8)|den4; }

                                    if (den != 0)
                                    {
                                        float fval = (float)num / den;
                                        sprintf(out_fnumber, "%.1f", fval);
                                    }
                                }
                                fseek(dosya, s_back, SEEK_SET);
                            }
                        }
                    }
                    fseek(dosya, e1_start + uzunluk - 2, SEEK_SET);
                }
                else
                {
                    fseek(dosya, e1_start + uzunluk - 2, SEEK_SET);
                }
            }
            else if (m2 == 0xE0)
            {
                fseek(dosya, 5, SEEK_CUR);
                fgetc(dosya);
                fgetc(dosya);
                out_unit = fgetc(dosya);
                
                int x1 = fgetc(dosya);
                int x2 = fgetc(dosya);
                out_x_density = (x1 << 8) | x2;

                int y1 = fgetc(dosya);
                int y2 = fgetc(dosya);
                out_y_density = (y1 << 8) | y2;

                fseek(dosya, uzunluk - 2 - 12, SEEK_CUR);
            }
            else
            {
                fseek(dosya, uzunluk - 2, SEEK_CUR);
            }
        }

        printf("\nFile: %s\n", argv[1]);

        printf("\n[Basic JPEG Info]\n");
        
        printf("Width              : ");
        if (out_width != -1) 
        { 
            printf("%d\n", out_width);
        }
        else 
        { 
            printf("Not available\n");
        }

        printf("Height             : ");
        if (out_height != -1) 
        { 
            printf("%d\n", out_height); 
        }
        else 
        { 
            printf("Not available\n"); 
        }

        printf("JFIF X Density     : ");
        if (out_x_density != -1) 
        { 
            printf("%d\n", out_x_density);
        }
        else 
        { 
            printf("Not available\n");
        }

        printf("JFIF Y Density     : ");
        if (out_y_density != -1) 
        { 
            printf("%d\n", out_y_density);
        }
        else 
        { 
            printf("Not available\n"); 
        }

        printf("Density Unit       : ");
        if (out_unit == 1) 
        { 
            printf("dots per inch\n"); 
        }
        else if (out_unit == 2) 
        { 
            printf("dots per cm\n"); 
        }
        else if (out_unit == 0) 
        { 
            printf("None\n"); 
        }
        else 
        { 
            printf("Not available\n");
        }

        printf("\n[EXIF]\n");
        
        printf("Camera Make        : ");
        if (out_make[0] != '\0') 
        { 
            printf("%s\n", out_make);
        }
        else 
        { 
            printf("Not available\n");
        }

        printf("Camera Model       : ");
        if (out_model[0] != '\0') 
        { 
            printf("%s\n", out_model); 
        }
        else 
        { 
            printf("Not available\n");
        }

        printf("Date Taken         : ");
        if (out_date[0] != '\0') 
        { 
            printf("%s\n", out_date); 
        }
        else 
        { 
            printf("Not available\n"); 
        }

        printf("Modify Date        : ");
        if (out_modify_date[0] != '\0') 
        { 
            printf("%s\n", out_modify_date); 
        }
        else 
        { 
            printf("Not available\n"); 
        }

        printf("Software           : ");
        if (out_software[0] != '\0') 
        { 
            printf("%s\n", out_software);
        }
        else 
        { 
            printf("Not available\n"); 
        }

        printf("ISO                : ");
        if (out_iso[0] != '\0') 
        { 
            printf("%s\n", out_iso);
        }
        else 
        { 
            printf("Not available\n"); 
        }

        printf("Exposure Time      : ");
        if (out_exposure[0] != '\0') 
        { 
            printf("%s\n", out_exposure);
        }
        else 
        { 
            printf("Not available\n"); 
        }

        printf("F Number           : ");
        if (out_fnumber[0] != '\0') 
        { 
            printf("%s\n", out_fnumber);
        }
        else 
        { 
            printf("Not available\n"); 
        }

        printf("\n[XMP]\n");
        printf("Creator Tool       : ");
        if (xmp_creator[0] != '\0') 
        { 
            printf("%s\n", xmp_creator);
        }
        else 
        { 
            printf("Not available\n");
        }

        printf("Metadata Date      : ");
        if (xmp_date[0] != '\0') 
        { 
            printf("%s\n", xmp_date);
        }
        else 
        { 
            printf("Not available\n");
        }

        printf("Label              : ");
        if (xmp_label[0] != '\0') 
        { 
            printf("%s\n", xmp_label);
        }
        else 
        { 
            printf("Not available\n");
        }

        printf("\n[Comment]\n");
        printf("Text               : ");
        if (out_comment[0] != '\0') 
        { 
            printf("%s\n", out_comment);
        }
        else 
        { 
            printf("Not available\n");
        }

        printf("\n[ICC]\n");
        printf("ICC Profile        : ");
        if (has_icc) 
        { 
            printf("Present\n");
        }
        else 
        {
            printf("Not available\n"); 
        }

        fclose(dosya);
        return 0;
    }