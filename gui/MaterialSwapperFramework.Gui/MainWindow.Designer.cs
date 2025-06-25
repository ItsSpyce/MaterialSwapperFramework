namespace MaterialSwapperFramework.Gui
{
    partial class MainWindow
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

    #region Windows Form Designer generated code

    /// <summary>
    ///  Required method for Designer support - do not modify
    ///  the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      ToolbarMenu = new MenuStrip();
      fileToolStripMenuItem = new ToolStripMenuItem();
      newToolStripMenuItem = new ToolStripMenuItem();
      mSFJSONToolStripMenuItem = new ToolStripMenuItem();
      mSFMaterialToolStripMenuItem = new ToolStripMenuItem();
      bGSMMaterialToolStripMenuItem = new ToolStripMenuItem();
      bGEMMaterialToolStripMenuItem = new ToolStripMenuItem();
      loadMSFJSONToolStripMenuItem = new ToolStripMenuItem();
      saveToolStripMenuItem = new ToolStripMenuItem();
      saveAsToolStripMenuItem = new ToolStripMenuItem();
      toolStripSeparator2 = new ToolStripSeparator();
      toolStripMenuItem1 = new ToolStripMenuItem();
      toolStripSeparator1 = new ToolStripSeparator();
      changeDataDirectoryToolStripMenuItem = new ToolStripMenuItem();
      exitToolStripMenuItem = new ToolStripMenuItem();
      editToolStripMenuItem = new ToolStripMenuItem();
      viewToolStripMenuItem = new ToolStripMenuItem();
      toolsToolStripMenuItem = new ToolStripMenuItem();
      aboutToolStripMenuItem = new ToolStripMenuItem();
      tableLayoutPanel1 = new TableLayoutPanel();
      statusStrip1 = new StatusStrip();
      statusLabel = new ToolStripStatusLabel();
      splitContainer1 = new SplitContainer();
      pluginTreeView = new TreeView();
      objectListView1 = new BrightIdeasSoftware.ObjectListView();
      ToolbarMenu.SuspendLayout();
      tableLayoutPanel1.SuspendLayout();
      statusStrip1.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)splitContainer1).BeginInit();
      splitContainer1.Panel1.SuspendLayout();
      splitContainer1.Panel2.SuspendLayout();
      splitContainer1.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)objectListView1).BeginInit();
      SuspendLayout();
      // 
      // ToolbarMenu
      // 
      ToolbarMenu.Items.AddRange(new ToolStripItem[] { fileToolStripMenuItem, editToolStripMenuItem, viewToolStripMenuItem, toolsToolStripMenuItem, aboutToolStripMenuItem });
      ToolbarMenu.Location = new Point(0, 0);
      ToolbarMenu.Name = "ToolbarMenu";
      ToolbarMenu.Size = new Size(800, 24);
      ToolbarMenu.TabIndex = 0;
      ToolbarMenu.Text = "menuStrip1";
      // 
      // fileToolStripMenuItem
      // 
      fileToolStripMenuItem.DropDownItems.AddRange(new ToolStripItem[] { newToolStripMenuItem, loadMSFJSONToolStripMenuItem, saveToolStripMenuItem, saveAsToolStripMenuItem, toolStripSeparator2, toolStripMenuItem1, toolStripSeparator1, changeDataDirectoryToolStripMenuItem, exitToolStripMenuItem });
      fileToolStripMenuItem.Name = "fileToolStripMenuItem";
      fileToolStripMenuItem.Size = new Size(37, 20);
      fileToolStripMenuItem.Text = "File";
      // 
      // newToolStripMenuItem
      // 
      newToolStripMenuItem.DropDownItems.AddRange(new ToolStripItem[] { mSFJSONToolStripMenuItem, mSFMaterialToolStripMenuItem, bGSMMaterialToolStripMenuItem, bGEMMaterialToolStripMenuItem });
      newToolStripMenuItem.Name = "newToolStripMenuItem";
      newToolStripMenuItem.Size = new Size(200, 22);
      newToolStripMenuItem.Text = "New";
      // 
      // mSFJSONToolStripMenuItem
      // 
      mSFJSONToolStripMenuItem.Name = "mSFJSONToolStripMenuItem";
      mSFJSONToolStripMenuItem.Size = new Size(152, 22);
      mSFJSONToolStripMenuItem.Text = "MSF JSON";
      // 
      // mSFMaterialToolStripMenuItem
      // 
      mSFMaterialToolStripMenuItem.Name = "mSFMaterialToolStripMenuItem";
      mSFMaterialToolStripMenuItem.Size = new Size(152, 22);
      mSFMaterialToolStripMenuItem.Text = "MSF Material";
      // 
      // bGSMMaterialToolStripMenuItem
      // 
      bGSMMaterialToolStripMenuItem.Name = "bGSMMaterialToolStripMenuItem";
      bGSMMaterialToolStripMenuItem.Size = new Size(152, 22);
      bGSMMaterialToolStripMenuItem.Text = "BGSM Material";
      // 
      // bGEMMaterialToolStripMenuItem
      // 
      bGEMMaterialToolStripMenuItem.Name = "bGEMMaterialToolStripMenuItem";
      bGEMMaterialToolStripMenuItem.Size = new Size(152, 22);
      bGEMMaterialToolStripMenuItem.Text = "BGEM Material";
      // 
      // loadMSFJSONToolStripMenuItem
      // 
      loadMSFJSONToolStripMenuItem.Name = "loadMSFJSONToolStripMenuItem";
      loadMSFJSONToolStripMenuItem.Size = new Size(200, 22);
      loadMSFJSONToolStripMenuItem.Text = "Open...";
      // 
      // saveToolStripMenuItem
      // 
      saveToolStripMenuItem.Name = "saveToolStripMenuItem";
      saveToolStripMenuItem.Size = new Size(200, 22);
      saveToolStripMenuItem.Text = "Save";
      // 
      // saveAsToolStripMenuItem
      // 
      saveAsToolStripMenuItem.Name = "saveAsToolStripMenuItem";
      saveAsToolStripMenuItem.Size = new Size(200, 22);
      saveAsToolStripMenuItem.Text = "Save As...";
      // 
      // toolStripSeparator2
      // 
      toolStripSeparator2.Name = "toolStripSeparator2";
      toolStripSeparator2.Size = new Size(197, 6);
      // 
      // toolStripMenuItem1
      // 
      toolStripMenuItem1.Name = "toolStripMenuItem1";
      toolStripMenuItem1.Size = new Size(200, 22);
      toolStripMenuItem1.Text = "Recent files";
      // 
      // toolStripSeparator1
      // 
      toolStripSeparator1.Name = "toolStripSeparator1";
      toolStripSeparator1.Size = new Size(197, 6);
      // 
      // changeDataDirectoryToolStripMenuItem
      // 
      changeDataDirectoryToolStripMenuItem.Name = "changeDataDirectoryToolStripMenuItem";
      changeDataDirectoryToolStripMenuItem.Size = new Size(200, 22);
      changeDataDirectoryToolStripMenuItem.Text = "Change data directory...";
      changeDataDirectoryToolStripMenuItem.Click += OnChangeDirectoryClicked;
      // 
      // exitToolStripMenuItem
      // 
      exitToolStripMenuItem.Name = "exitToolStripMenuItem";
      exitToolStripMenuItem.Size = new Size(200, 22);
      exitToolStripMenuItem.Text = "Exit";
      // 
      // editToolStripMenuItem
      // 
      editToolStripMenuItem.Name = "editToolStripMenuItem";
      editToolStripMenuItem.Size = new Size(39, 20);
      editToolStripMenuItem.Text = "Edit";
      // 
      // viewToolStripMenuItem
      // 
      viewToolStripMenuItem.Name = "viewToolStripMenuItem";
      viewToolStripMenuItem.Size = new Size(44, 20);
      viewToolStripMenuItem.Text = "View";
      // 
      // toolsToolStripMenuItem
      // 
      toolsToolStripMenuItem.Name = "toolsToolStripMenuItem";
      toolsToolStripMenuItem.Size = new Size(46, 20);
      toolsToolStripMenuItem.Text = "Tools";
      // 
      // aboutToolStripMenuItem
      // 
      aboutToolStripMenuItem.Name = "aboutToolStripMenuItem";
      aboutToolStripMenuItem.Size = new Size(52, 20);
      aboutToolStripMenuItem.Text = "About";
      // 
      // tableLayoutPanel1
      // 
      tableLayoutPanel1.AutoSizeMode = AutoSizeMode.GrowAndShrink;
      tableLayoutPanel1.ColumnCount = 1;
      tableLayoutPanel1.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 50F));
      tableLayoutPanel1.Controls.Add(statusStrip1, 0, 1);
      tableLayoutPanel1.Controls.Add(splitContainer1, 0, 0);
      tableLayoutPanel1.Dock = DockStyle.Fill;
      tableLayoutPanel1.Location = new Point(0, 24);
      tableLayoutPanel1.Name = "tableLayoutPanel1";
      tableLayoutPanel1.RowCount = 2;
      tableLayoutPanel1.RowStyles.Add(new RowStyle(SizeType.Percent, 50F));
      tableLayoutPanel1.RowStyles.Add(new RowStyle(SizeType.Absolute, 20F));
      tableLayoutPanel1.Size = new Size(800, 426);
      tableLayoutPanel1.TabIndex = 2;
      // 
      // statusStrip1
      // 
      statusStrip1.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
      statusStrip1.Dock = DockStyle.None;
      statusStrip1.Items.AddRange(new ToolStripItem[] { statusLabel });
      statusStrip1.Location = new Point(0, 406);
      statusStrip1.Name = "statusStrip1";
      statusStrip1.Size = new Size(800, 20);
      statusStrip1.TabIndex = 2;
      statusStrip1.Text = "statusStrip1";
      // 
      // statusLabel
      // 
      statusLabel.Name = "statusLabel";
      statusLabel.Size = new Size(0, 15);
      // 
      // splitContainer1
      // 
      splitContainer1.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
      splitContainer1.Location = new Point(3, 3);
      splitContainer1.Name = "splitContainer1";
      // 
      // splitContainer1.Panel1
      // 
      splitContainer1.Panel1.Controls.Add(pluginTreeView);
      // 
      // splitContainer1.Panel2
      // 
      splitContainer1.Panel2.Controls.Add(objectListView1);
      splitContainer1.Size = new Size(794, 400);
      splitContainer1.SplitterDistance = 244;
      splitContainer1.TabIndex = 3;
      // 
      // pluginTreeView
      // 
      pluginTreeView.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
      pluginTreeView.Font = new Font("Consolas", 9F);
      pluginTreeView.Location = new Point(0, 0);
      pluginTreeView.Margin = new Padding(0);
      pluginTreeView.Name = "pluginTreeView";
      pluginTreeView.Size = new Size(241, 400);
      pluginTreeView.TabIndex = 0;
      // 
      // objectListView1
      // 
      objectListView1.Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
      objectListView1.Font = new Font("Consolas", 9F, FontStyle.Regular, GraphicsUnit.Point, 0);
      objectListView1.Location = new Point(0, 0);
      objectListView1.Margin = new Padding(0);
      objectListView1.Name = "objectListView1";
      objectListView1.Size = new Size(543, 400);
      objectListView1.TabIndex = 1;
      objectListView1.View = View.Details;
      // 
      // MainWindow
      // 
      AutoScaleDimensions = new SizeF(7F, 15F);
      AutoScaleMode = AutoScaleMode.Font;
      ClientSize = new Size(800, 450);
      Controls.Add(tableLayoutPanel1);
      Controls.Add(ToolbarMenu);
      MainMenuStrip = ToolbarMenu;
      MaximizeBox = false;
      Name = "MainWindow";
      Text = "MSF GUI";
      Load += OnLoad;
      ToolbarMenu.ResumeLayout(false);
      ToolbarMenu.PerformLayout();
      tableLayoutPanel1.ResumeLayout(false);
      tableLayoutPanel1.PerformLayout();
      statusStrip1.ResumeLayout(false);
      statusStrip1.PerformLayout();
      splitContainer1.Panel1.ResumeLayout(false);
      splitContainer1.Panel2.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)splitContainer1).EndInit();
      splitContainer1.ResumeLayout(false);
      ((System.ComponentModel.ISupportInitialize)objectListView1).EndInit();
      ResumeLayout(false);
      PerformLayout();
    }

    #endregion

    private MenuStrip ToolbarMenu;
    private ToolStripMenuItem fileToolStripMenuItem;
    private ToolStripMenuItem editToolStripMenuItem;
    private ToolStripMenuItem viewToolStripMenuItem;
    private ToolStripMenuItem toolsToolStripMenuItem;
    private ToolStripMenuItem aboutToolStripMenuItem;
    private ToolStripMenuItem loadMSFJSONToolStripMenuItem;
    private ToolStripMenuItem saveToolStripMenuItem;
    private ToolStripMenuItem saveAsToolStripMenuItem;
    private ToolStripMenuItem newToolStripMenuItem;
    private ToolStripMenuItem mSFJSONToolStripMenuItem;
    private ToolStripMenuItem mSFMaterialToolStripMenuItem;
    private ToolStripMenuItem bGSMMaterialToolStripMenuItem;
    private ToolStripMenuItem bGEMMaterialToolStripMenuItem;
    private ToolStripSeparator toolStripSeparator2;
    private ToolStripMenuItem toolStripMenuItem1;
    private ToolStripSeparator toolStripSeparator1;
    private ToolStripMenuItem exitToolStripMenuItem;
    private TableLayoutPanel tableLayoutPanel1;
    private StatusStrip statusStrip1;
    private ToolStripStatusLabel statusLabel;
    private ToolStripMenuItem changeDataDirectoryToolStripMenuItem;
    private SplitContainer splitContainer1;
    private TreeView pluginTreeView;
    private BrightIdeasSoftware.ObjectListView objectListView1;
  }
}
