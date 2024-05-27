 <%@ Page Language="C#" AutoEventWireup="true" CodeBehind="GroupValidation_zxn_10.aspx.cs" Inherits="ExSite_zxn_10.Ex5_zxn_10.GroupValidation_zxn_10" %>

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
            <asp:Button ID="btnValiateName" runat="server" Text="用户名是否可用" ValidationGroup="groupName" OnClick="btnValiateName_Click" />
            <asp:Label ID="lblName" runat="server" Text=""></asp:Label>
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
            <asp:RangeValidator ID="rvBirthday" runat="server" ErrorMessage="日期应在1900-1-1到2021-1-1之间！" ControlToValidate="txtBirthday" Display="Dynamic"  MaximumValue="2020-1-1" MinimumValue="1900-1-1" SetFocusOnError="true" Type="Date"></asp:RangeValidator>
            <br />
            <asp:Label ID="Label9" runat="server" Text="电话号码：" Width="80px"></asp:Label>
            <asp:TextBox ID="txtTelephone" runat="server"></asp:TextBox>
            <asp:RequiredFieldValidator ID="rfvhone" runat="server" ErrorMessage="请输入电话号码！" ControlToValidate="txtTelephone" Display="Dynamic" SetFocusOnError="true" >*</asp:RequiredFieldValidator>   
            <asp:RegularExpressionValidator ID="revTelephone" runat="server" ErrorMessage="电话号码格式应为021-66798304！" ControlToValidate="txtTelephone" Display="Dynamic" ValidationExpression="\d{3}-\d{8}" SetFocusOnError="true"></asp:RegularExpressionValidator>
            <br />
            <asp:Label ID="Label11" runat="server" Text="身份证号：" Width ="80px"></asp:Label>
            <asp:TextBox ID="txtIdentity" runat="server"></asp:TextBox>
            <asp:RequiredFieldValidator ID="rfvIdentity" runat="server" ErrorMessage="请输入身份证号！" ControlToValidate="txtIdentity" Display="Dynamic" SetFocusOnError="true">*</asp:RequiredFieldValidator>
            <asp:CustomValidator ID="csvIdentity" runat="server" ErrorMessage="身份证号错误！" ControlToValidate="txtIdentity" Display="Dynamic"  SetFocusOnError="true" OnServerValidate="CustomValidator1_ServerValidate"></asp:CustomValidator>
        </div>
        <asp:Button ID="btnSubmit" runat="server" Text="确定" OnClick="Button1_Click" Width="50px" />
        <br />
        <asp:Label ID="lblMsg" runat="server" Width="80px"></asp:Label>
        <br />
        <asp:ValidationSummary ID="vsSubmit" runat="server"   ShowMessageBox="true" ShowSummary="false"/>
        <asp:ValidationSummary ID="vsName" runat="server" Disply="Dynamic" ShowMessageBox="true" ShowSummary="false" ValidationGroup="groupName" />
    </form>
</body>
</html>
