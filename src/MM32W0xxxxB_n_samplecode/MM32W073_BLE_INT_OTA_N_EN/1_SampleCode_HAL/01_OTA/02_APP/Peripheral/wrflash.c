#include "wrflash.h"
#include "delay.h"
#include "uart.h"

//////////////////////////////////////////////////////////////////////////////////
//103������
//FLASH ��������
//////////////////////////////////////////////////////////////////////////////////

//��ȡָ����ַ�İ���(16λ����)
//faddr:����ַ(�˵�ַ����Ϊ2�ı���!!)
//����ֵ:��Ӧ����.
u16 ONCHIP_FLASH_ReadHalfWord(u32 faddr)
{
  return *(vu16 *)faddr;
}
#if ONCHIP_FLASH_WREN //���ʹ����д   
//������д��
//WriteAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��
void ONCHIP_FLASH_Write_NoCheck(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite)
{
  u16 i;
  for (i = 0; i < NumToWrite; i++)
  {
    FLASH_ProgramHalfWord(WriteAddr, pBuffer[i]);
    WriteAddr += 2; //��ַ����2.
  }
}
//��ָ����ַ��ʼд��ָ�����ȵ�����
//WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)
//pBuffer:����ָ��
//NumToWrite:����(16λ)��(����Ҫд���16λ���ݵĸ���.)
#if ONCHIP_FLASH_SIZE<256
  #define ONCHIP_FLASH_SECTOR_SIZE 1024 //�ֽ�
#else
  #define ONCHIP_FLASH_SECTOR_SIZE  2048
#endif
u16 ONCHIP_FLASH_BUF[ONCHIP_FLASH_SECTOR_SIZE / 2]; //�����2K�ֽ�
void ONCHIP_FLASH_Write(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite)
{
  u32 Secpos;    //������ַ
  u16 Secoff;    //������ƫ�Ƶ�ַ(16λ�ּ���)
  u16 Secremain; //������ʣ���ַ(16λ�ּ���)
  u16 i;
  u32 Offaddr;   //ȥ��0X08000000��ĵ�ַ
  if (WriteAddr < ONCHIP_FLASH_BASE || (WriteAddr >= (ONCHIP_FLASH_BASE + 1024 * ONCHIP_FLASH_SIZE)))return; //�Ƿ���ַ
  FLASH_Unlock();           //����
  Offaddr = WriteAddr - ONCHIP_FLASH_BASE; //ʵ��ƫ�Ƶ�ַ.
  Secpos = Offaddr / ONCHIP_FLASH_SECTOR_SIZE;  //������ַ  0~127 for 103RBT6
  Secpos = (Offaddr % ONCHIP_FLASH_SECTOR_SIZE) / 2; //�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
  Secremain = ONCHIP_FLASH_SECTOR_SIZE / 2 - Secpos; //����ʣ��ռ��С
  if (NumToWrite <= Secremain)Secremain = NumToWrite; //�����ڸ�������Χ
  while (1)
  {
    ONCHIP_FLASH_Read(Secpos * ONCHIP_FLASH_SECTOR_SIZE + ONCHIP_FLASH_BASE, ONCHIP_FLASH_BUF, ONCHIP_FLASH_SECTOR_SIZE / 2); //������������������
    for (i = 0; i < Secremain; i++) //У������
    {
      if (ONCHIP_FLASH_BUF[Secoff + i] != 0XFFFF)break; //��Ҫ����
    }
    if (i < Secremain) //��Ҫ����
    {
      FLASH_ErasePage(Secpos * ONCHIP_FLASH_SECTOR_SIZE + ONCHIP_FLASH_BASE); //�����������
      for (i = 0; i < Secremain; i++) //����
      {
        ONCHIP_FLASH_BUF[i + Secoff] = pBuffer[i];
      }
      ONCHIP_FLASH_Write_NoCheck(Secpos * ONCHIP_FLASH_SECTOR_SIZE + ONCHIP_FLASH_BASE, ONCHIP_FLASH_BUF, ONCHIP_FLASH_SECTOR_SIZE / 2); //д����������
    }
    else ONCHIP_FLASH_Write_NoCheck(WriteAddr, pBuffer, Secremain); //д�Ѿ������˵�,ֱ��д������ʣ������.
    if (NumToWrite == Secremain)break; //д�������
    else//д��δ����
    {
      Secpos++;       //������ַ��1
      Secoff = 0;     //ƫ��λ��Ϊ0
      pBuffer += Secremain; //ָ��ƫ��
      WriteAddr = Secpos * ONCHIP_FLASH_SECTOR_SIZE + ONCHIP_FLASH_BASE; //WriteAddr+=secremain; //д��ַƫ��
      NumToWrite -= Secremain; //�ֽ�(16λ)���ݼ�
      if (NumToWrite > (ONCHIP_FLASH_SECTOR_SIZE / 2))Secremain = ONCHIP_FLASH_SECTOR_SIZE / 2; //��һ����������д����
      else Secremain = NumToWrite; //��һ����������д����
    }
  };
  FLASH_Lock();//����
}
#endif

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��
void ONCHIP_FLASH_Read(u32 ReadAddr, u16 *pBuffer, u16 NumToRead)
{
  u16 i;
  for (i = 0; i < NumToRead; i++)
  {
    pBuffer[i] = ONCHIP_FLASH_ReadHalfWord(ReadAddr); //��ȡ2���ֽ�.
    ReadAddr += 2; //ƫ��2���ֽ�.
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//WriteAddr:��ʼ��ַ
//WriteData:Ҫд�������
void Test_Write(u32 WriteAddr, u16 WriteData)
{
  ONCHIP_FLASH_Write(WriteAddr, &WriteData, 1); //д��һ����
}
















