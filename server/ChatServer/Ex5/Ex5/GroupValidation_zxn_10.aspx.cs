using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

namespace ExSite_zxn_10.Ex5_zxn_10
{
    public partial class GroupValidation_zxn_10 : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            UnobtrusiveValidationMode = UnobtrusiveValidationMode.None;
        }

        protected void CustomValidator1_ServerValidate(object source, ServerValidateEventArgs args)
        {
            string cid = args.Value;
            args.IsValid = true;
            try
            { DateTime.Parse(cid.Substring(6, 4) + "-" + cid.Substring(10, 2) + "-" + cid.Substring(12, 2)); }
            catch
            {
                args.IsValid = false;
            }
        }
        protected void Button1_Click(object sender, EventArgs e)
        {
            lblMsg.Text = "";
            if (Page.IsValid)
            {
                lblMsg.Text = "验证通过！";
            }
        }

        protected void btnValiateName_Click(object sender, EventArgs e)
        {
            if(txtName.Text == "leaf")
            {
                lblName.Text = "抱歉！该用户名已被占用！";
            }
            else
            {
                lblName.Text = "恭喜！该用户名可用！";
            }
        }
    }
}
