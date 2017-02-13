' **********************************************************************
'
' Copyright (c) 2003-2017 ZeroC, Inc. All rights reserved.
'
' **********************************************************************

Imports Demo
Imports System

Public NotInheritable Class CallbackReceiverI
    Inherits CallbackReceiverDisp_

    Public Overloads Overrides Sub callback(ByVal current As Ice.Current)
        Console.WriteLine("received callback")
    End Sub

End Class
