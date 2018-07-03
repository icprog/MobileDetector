﻿using common.utils.Interfaces;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;

namespace common.utils.Files
{
	public static class Files
	{
		public static bool CopyFile(string srcFName, string dstFName, IProgressInfo pInfo)
		{
			if (string.IsNullOrEmpty(srcFName))
				return false;
			//
			if (!File.Exists(srcFName))
				return false;
			//
			FileInfo fi = new FileInfo(srcFName);
			int size = (int)fi.Length;
			int step;
			int wrLen;
			if (size < 1000)
			{
				step = 100;
				wrLen = size;
			}
			else
			{
				step = 1;
				wrLen = size / 100;
			}
			//
			double percentage = 0;
			if (pInfo != null)
			{
				pInfo.Percentage = percentage;
				pInfo.Description = "Копирование файла <" + fi.Name + ">";
			}
			//
			FileStream fsSrc = null;
			FileStream fsDst = null;
			try
			{
				fsSrc = new FileStream(srcFName, FileMode.Open, FileAccess.Read);
				fsDst = File.Create(dstFName, wrLen);
				byte[] buf = new byte[wrLen];
				int rdLen = 0;
				//
				while (size > 0)
				{
					rdLen = fsSrc.Read(buf, 0, buf.Length);
					fsDst.Write(buf, 0, rdLen);
					fsDst.Flush();
					size -= rdLen;
					percentage += step;
					if (pInfo != null)
						pInfo.Percentage = percentage;
					Thread.Sleep(5);
				}
				//
				Thread.Sleep(100);
				percentage = 100;
				if (pInfo != null)
				{
					pInfo.Percentage = percentage;
					pInfo.Description = "Копирование файла <" + fi.Name + "> : Ok";
					Thread.Sleep(200);
				}
				//
				return true;
			}
			catch
			{
				if (pInfo != null)
				{
					pInfo.Percentage = 0;
					pInfo.Description = "<" + fi.Name + "> : Error";
					Thread.Sleep(200);
				}
				return false;
			}
			finally
			{
				if (fsSrc != null)
				{
					fsSrc.Close();
				}
				//
				if (fsDst != null)
				{
					fsDst.Close();
				}
			}
		}
	}
}