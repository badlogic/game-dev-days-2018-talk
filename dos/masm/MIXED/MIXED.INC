d If
End Sub

Private Sub Form_Load()
    AlarmTime = ""
End Sub

Private Sub Form_Resize()
    If WindowState = conMinimized Then      ' If form is minimized, display the time in a caption.
        SetCaptionTime
    Else
        Caption = "Alarm Clock"
    End If
End Sub

Private Sub SetCaptionTime()
    Caption = Format(Time, "Medium Time")   ' Display time using medium time format.
End Sub

Private Sub Timer1_Timer()
Static AlarmSounded As Integer
    If lblTime.Caption <> CStr(Time) Then
        ' It's now a different second than the one displayed.
        If Time >= AlarmTime And Not AlarmSounded Then
            Beep
            MsgBox "Alarm at " & Time
            AlarmSounded = True
        ElseIf Time < AlarmTime Then
            AlarmSounded = False
        End If
        If WindowState = conMinimized Then
            ' If minimized, then update the form's Caption every minute.
            If Minute(CDate(Caption)) <> Minute(Time) Then SetCaptionTime
        Else
            ' Otherwise, update the label Caption in the form every second.
            lblTime.Caption = Time
        End If
    End If
End Sub

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        	{u�-����m~���   ��cC$i1xv�=��wq��w�_����ݣi;����o��������_���~���߿�ZP�U���'x�RN�+X�~�#9�,},��Ƹԫ
��_��5���{�XS:������#���ɯ�^��±��Bma���;�W����]F��^�2���|��5w~fҡ��{A���dW6���N�7�kVA������ϋ�c)y��I��	ƷD_*6L���3#V��#B=�,�ԣm=;$�h��Hӗ��n��@d�z��ZH��[*�(;�c�c3F��rw,k�R'F1ص��˜Á�:=D�{2��&9�75$X�ֶb~��X�^q�G*6c2E+�jH�c$��8vAE>�%���� =������Œv�Y�)y,N��0�˛8V��_V/�����}6ɤ�d��++g\�Y��:�e���������/��\BGX�$&�Y�б�:��r�M��o%܊���$�օ������K��8�̖$�ـ�8Ƞ�a5�I�|o�A���߀�4��x�����)��FU��
4˟���R,�kt���5o�����źPի�\�D�:$�@�z����]+ ��'"��A�Wv���l�Vs�pEï�F��3��,з\
!Y�E+���j�&��s{RaڒT2�o�D�ǘ 1�D)	�;�ڪL���d0A\$�wj����8��"m�퐧�����M����+���B�����^W|�lXAû���G�F����h<�ެ\��X�Ra��G7;;��� �! /��$_;"�:Xv$�R��38���(���@=���-���~��P���54@��)<O�XH>�RQȳ�p�\�1Ћ��NS��'���H���*� �Y@>*����@h]ɪ��^�lj�6	7[�y`>P{>�>i⾭a��5���ͥ�wKڗ��Q����&V�j�_�(��.�sQR(O��E�"�w5'-�i$S�"��O"^%�$� ܉DFD�7�wb�� ����W8*Hu���+)���ZR0��,_�q�U|fE�P�g��Y|'�7���4NoL����ƃ���	�ck��=�8Vv��$[x�?EfM˧F�