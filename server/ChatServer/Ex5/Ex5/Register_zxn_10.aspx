<%@ Page Language="C#" AutoEventWireup="true" CodeBehind="Register_zxn_10.aspx.cs" Inherits="ExSite_zxn_10.Ex5_zxn_10.Register_zxn_10" %>

<!DOCTYPE html>

<html xmlns="http://www.w3.org/1999/xhtml">
<head runat="server">
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
    <title></title>
</head>
<body>
    <form id="form1" runat="server">
        <div>
            <asp:Label ID="Label1" runat="server" Text="用户名：" Width="80px"></asp:Label>
            <asp:TextBox ID="txtName" runat="server"></asp:TextBox>
            <asp:RequiredFieldValidator ID="rfvName" runat="server" ErrorMessage="请输入用户名！" ControlToValidate="txtName" Display="Dynamic"  SetFocusOnError="true" >*</asp:RequiredFieldValidator>
            <br />
            <asp:Label ID="Label3" runat="server" Text="密码：" Width="80px"></asp:Label>
            <asp:TextBox ID="txtPassword" runat="server" TextMode="Password"></asp:TextBox>
            <asp:RequiredFieldValidator ID="rfvPassword" runat="server" ErrorMessage="请输入密码！" Display="Dynamic" ControlToValidate="txtPassword" SetFocusOnError="true" >*</asp:RequiredFieldValidator>
            <br />
            <asp:Label ID="Label5" runat="server" Text="确认密码：" Width="80px"></asp:Label>
            <asp:TextBox ID="txtPasswordAgain" runat="server" TextMode="Password"></asp:TextBox>
            <asp:RequiredFieldValidator ID="rfvPasswordAgain" runat="server" ErrorMessage="请输入确认密码！" ControlToValidate="txtPasswordAgain" Display="Dynamic" SetFocusOnError="true">*</asp:RequiredFieldValidator>
            <asp:CompareValidator ID="cvPassword" runat="server" ErrorMessage="密码与确认密码不一致！" ControlToCompare="txtPassword" ControlToValidate="txtPasswordAgain" Display="Dynamic" SetFocusOnError="true" ></asp:CompareValidator>
            <br />
            <asp:Label ID="Label7" runat="server" Text="生日：" Width="80px"></asp:Label>
            <asp:TextBox ID="txtBirthday" runat="server"></asp:TextBox>
            <asp:RequiredFieldValidator ID="rfvBirthday" runat="server" ErrorMessage="请输入生日！" ControlToValidate="txtBirthday" Display="Dynamic" SetFocusOnError="true">*</asp:RequiredFieldValidator>
            <asp:RangeValidator ID="rvBirthday" runat="server" ErrorMessage="日期应在1900-1-1到至今之间！" ControlToValidate="txtBirthday" Display="Dynamic"   MinimumValue="1900-1-1" SetFocusOnError="true" Type="Date"></asp:RangeValidator>
            <br />
            <asp:Label ID="Label9" runat="server" Text="电话号码：" Width="80px"></asp:Label>
            <asp:TextBox ID="txtTelephone" runat="server"></asp:TextBox>
            <asp:RequiredFieldValidator ID="rfvhone" runat="server" ErrorMessage="请输入电话号码！" ControlToValidate="txtTelephone" Display="Dynamic" SetFocusOnError="true" >*</asp:RequiredFieldValidator>   
            <asp:RegularExpressionValidator ID="revTelephone" runat="server" ErrorMessage="电话号码格式不正确！" ControlToValidate="txtTelephone" Display="Dynamic" SetFocusOnError="true"  
    ValidationExpression="^\(?0\d{2,4}\)?[- ]?\d{7,8}$">  
</asp:RegularExpressionValidator>
            <br />
            <asp:Label ID="Label11" runat="server" Text="身份证号：" Width ="80px"></asp:Label>
            <asp:TextBox ID="txtIdentity" runat="server"></asp:TextBox>
           <asp:CustomValidator ID="CustomValidatorIDCard" runat="server" ErrorMessage="身份证号格式不正确或校验码错误！" ControlToValidate="txtIdentity" Display="Dynamic" SetFocusOnError="true" OnServerValidate="CustomValidatorIDCard_ServerValidate"></asp:CustomValidator>
            <asp:CustomValidator ID="csvIdentity" runat="server" ErrorMessage="身份证号错误！" ControlToValidate="txtIdentity" Display="Dynamic"  SetFocusOnError="true" OnServerValidate="CustomValidator1_ServerValidate"></asp:CustomValidator>
        </div>
        <asp:Button ID="btnSubmit" runat="server" Text="确定" OnClick="Button1_Click" Width="50px" />
        <br />
        <asp:Label ID="lblMsg" runat="server" Width="80px"></asp:Label>
        <br />
        <asp:ValidationSummary ID="vsSubmit" runat="server"   ShowMessageBox="true" ShowSummary="false"/>
    </form>
</body>
</html>
