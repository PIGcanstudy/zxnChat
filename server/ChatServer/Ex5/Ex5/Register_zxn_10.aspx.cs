using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.UI;
using System.Web.UI.WebControls;

namespace ExSite_zxn_10.Ex5_zxn_10
{
    public partial class Register_zxn_10 : System.Web.UI.Page
    {
        protected void Page_Load(object sender, EventArgs e)
        {
            UnobtrusiveValidationMode = UnobtrusiveValidationMode.None;
            rvBirthday.MaximumValue = DateTime.Now.ToString("yyyy-MM-dd");
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

        protected void CustomValidatorIDCard_ServerValidate(object source, ServerValidateEventArgs args)
        {
            string idCard = args.Value;
            if (string.IsNullOrEmpty(idCard) || idCard.Length != 18)
            {
                args.IsValid = false;
                return;
            }

            // 验证省份代码（这里只是一个示例，你需要实现完整的省份代码验证逻辑）  
            string provinceCode = idCard.Substring(0, 2);
            // TODO: 检查provinceCode是否在有效的省份代码列表中  

            // 验证校验码  
            bool isValidCheckDigit = ValidateIDCardCheckDigit(idCard.Substring(0, 17));
            args.IsValid = isValidCheckDigit; // 假设省份代码验证也总是通过，或者你可以在这里结合省份代码的验证结果  
        }

        private bool ValidateIDCardCheckDigit(string idCardBody)
        {
            int[] weights = { 7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2 };
            string checkCodes = "10X98765432";

            int sum = 0;
            for (int i = 0; i < idCardBody.Length; i++)
            {
                int number = int.Parse(idCardBody[i].ToString());
                sum += number * weights[i];
            }

            int remainder = sum % 11;
            return checkCodes[remainder] == AidCard[17].ToString().ToUpper();
        }

        protected void Button1_Click(object sender, EventArgs e)
        {
            lblMsg.Text = "";
            if (Page.IsValid)
            {
                lblMsg.Text = "验证通过！";
            }
        }

     
    }
}